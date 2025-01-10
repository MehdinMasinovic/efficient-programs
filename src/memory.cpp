#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <unordered_map>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <cmath>
#include <immintrin.h>  // For prefetching

typedef uint64_t encoded_string_t;

struct Record {
    encoded_string_t key_encoded;
    encoded_string_t value_encoded;
    std::string key;
    std::string value;
};

std::vector<Record> readCSV(const std::string& filename) {
    std::vector<Record> records;

    // Open file using low-level I/O
    int fd = open(filename.c_str(), O_RDONLY);
    if (fd == -1) {
        throw std::runtime_error("Cannot open file: " + filename);
    }

    // Get file size
    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        close(fd);
        throw std::runtime_error("Cannot get file size");
    }
    
    // Compute mapping size to align to page boundaries (reduces TLB misses)
    size_t file_size = sb.st_size;
    size_t page_size = sysconf(_SC_PAGESIZE);
    size_t mapping_size = ((file_size + page_size - 1) / page_size) * page_size;

    // Memory map the file
    const char* data = static_cast<const char*>(
        mmap(nullptr, mapping_size, PROT_READ, MAP_PRIVATE, fd, 0)
    );

    if (data == MAP_FAILED) {
        close(fd);
        throw std::runtime_error("Cannot mmap file");
    }

    // Pre-allocate space for records (estimate based on average line length)
    size_t estimated_records = sb.st_size / 30;  // Assume average line length of 30 bytes
    records.reserve(estimated_records);

    // Process the memory-mapped file
    const char* current = data;
    const char* end = data + sb.st_size;
    const char* line_start = current;

    while (current < end) {
        if (*current == '\n') {
            // Find comma in current line
            const char* comma = line_start;
            while (comma < current && *comma != ',') {
                ++comma;
            }

            if (comma < current) {
                Record record;
                record.key.assign(line_start, comma - line_start);
                record.value.assign(comma + 1, current - (comma + 1));
                record.key_encoded = std::hash<std::string>{}(record.key);
                record.value_encoded = std::hash<std::string>{}(record.value);
                records.push_back(std::move(record));
            }

            line_start = current + 1;
        }
        ++current;
    }

    // Handle last line if it doesn't end with newline
    if (line_start < end) {
        const char* comma = line_start;
        while (comma < end && *comma != ',') {
            ++comma;
        }

        if (comma < end) {
            Record record;
            record.key.assign(line_start, comma - line_start);
            record.value.assign(comma + 1, current - (comma + 1));
            record.key_encoded = std::hash<std::string>{}(record.key);
            record.value_encoded = std::hash<std::string>{}(record.value);
            records.push_back(std::move(record));
        }
    }

    // Cleanup
    munmap(const_cast<char*>(data), mapping_size);
    close(fd);

    return records;
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " file1.csv file2.csv file3.csv file4.csv\n";
        return 1;
    }

    try {
        // Read all files with buffered I/O
        auto file1 = readCSV(argv[1]);
        auto file2 = readCSV(argv[2]);
        auto file3 = readCSV(argv[3]);
        auto file4 = readCSV(argv[4]);

        // Create hash maps
        std::unordered_multimap<encoded_string_t, Record> file1_map;
        std::unordered_multimap<encoded_string_t, Record> file2_map;
        std::unordered_multimap<encoded_string_t, Record> file4_map;

        // Reserve space in hash maps
        file1_map.reserve(file1.size());
        file2_map.reserve(file2.size());
        file4_map.reserve(file4.size());

        // Build hash tables
        for (const auto& record : file1) {
            file1_map.emplace(record.key_encoded, record);
        }
        for (const auto& record : file2) {
            file2_map.emplace(record.key_encoded, record);
        }
        for (const auto& record : file4) {
            file4_map.emplace(record.key_encoded, record);
        }

        // Set up output buffering
        std::ios_base::sync_with_stdio(false);
        std::cout.tie(nullptr);
        char output_buffer[1024*1024];  // 1MB output buffer
        std::cout.rdbuf()->pubsetbuf(output_buffer, sizeof(output_buffer));

        // Perform join
        for (size_t i = 0; i < file3.size(); ++i) {
            const auto& f3_record = file3[i];

            // Prefetch next record into cache
            if (i + 1 < file3.size()) {
                _mm_prefetch(reinterpret_cast<const char*>(&file3[i + 1]), _MM_HINT_T0);
            }
            auto f4_range = file4_map.equal_range(f3_record.value_encoded);
            for (auto f4_it = f4_range.first; f4_it != f4_range.second; ++f4_it) {
                auto f1_range = file1_map.equal_range(f3_record.key_encoded);
                for (auto f1_it = f1_range.first; f1_it != f1_range.second; ++f1_it) {
                    auto f2_range = file2_map.equal_range(f3_record.key_encoded);
                    for (auto f2_it = f2_range.first; f2_it != f2_range.second; ++f2_it) {
                        std::cout << f4_it->second.key << ","
                                 << f1_it->second.key << ","
                                 << f1_it->second.value << ","
                                 << f2_it->second.value << ","
                                 << f4_it->second.value << "\n";
                    }
                }
            }
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
