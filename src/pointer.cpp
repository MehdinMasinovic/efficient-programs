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
    const char* key;
    size_t key_length;
    const char* value;
    size_t value_length;
};

void generateRecord (const char* key, size_t key_length, const char* value, size_t value_length, Record& record){
        record.key_encoded = std::hash<std::string>{}(std::string(key, key_length));
        record.value_encoded = std::hash<std::string>{}(std::string(value, value_length));
        record.key = key;
        record.key_length = key_length;
        record.value = value;
        record.value_length = value_length;
}

std::vector<Record> readCSV(struct stat sb, const char* data) {
    std::vector<Record> records;

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
                generateRecord(line_start, comma-line_start, comma+1, current-(comma+1), record);
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
            generateRecord(line_start, comma-line_start, comma+1, current-(comma+1), record);
            records.push_back(std::move(record));
        }
    }

    return records;
}

std::unordered_multimap<encoded_string_t, Record> readCSVAndFillMap(struct stat sb, const char* data) {
    std::unordered_multimap<encoded_string_t, Record> records;

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
                generateRecord(line_start, comma-line_start, comma+1, current-(comma+1), record);
                records.emplace(record.key_encoded, record);
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
            generateRecord(line_start, comma-line_start, comma+1, current-(comma+1), record);
            records.emplace(record.key_encoded, record);
        }
    }

    return records;
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " file1.csv file2.csv file3.csv file4.csv\n";
        return 1;
    }

    try {
        int fds [4];
        struct stat sbs[4];
        const char* data_arr [4];
        size_t string_table_size = 0;

        for (int i = 1; i < 5; i++) {
            // Open file using low-level I/O
            int fd = open(argv[i], O_RDONLY);
            if (fd == -1) {
                throw std::runtime_error("Cannot open file");
            }

            fds[i-1] = fd;

            // Get file size
            struct stat sb;
            if (fstat(fd, &sb) == -1) {
                close(fd);
                throw std::runtime_error("Cannot get file size");
            }

            sbs[i-1] = sb;
            string_table_size += sb.st_size / 30;
            // Memory map the file
            const char* data = static_cast<const char*>(
                mmap(nullptr, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0)
            );

            if (data == MAP_FAILED) {
                close(fd);
                throw std::runtime_error("Cannot mmap file");
            }

            data_arr[i-1] = data;
        }
        //read files and create data structures
        auto file1_map = readCSVAndFillMap(sbs[0], data_arr[0]);
        auto file2_map = readCSVAndFillMap(sbs[1], data_arr[1]);
        auto file3 = readCSV(sbs[2], data_arr[2]);
        auto file4_map = readCSVAndFillMap(sbs[3], data_arr[3]);

        // Set up output buffering
        std::ios_base::sync_with_stdio(false);
        std::cout.tie(nullptr);
        char output_buffer[1024*1024*3];  // 3MB output buffer
        std::cout.rdbuf()->pubsetbuf(output_buffer, sizeof(output_buffer));

        // Perform join
        for (size_t i = 0; i < file3.size(); ++i) {
            const auto& f3_record = file3[i];

            // Prefetch next record into cache
            if (i + 1 < file3.size()) {
                _mm_prefetch(reinterpret_cast<const char*>(&file3[i + 1]), _MM_HINT_T0);
            }

            // calculate ranges for each map
            auto f4_range = file4_map.equal_range(f3_record.value_encoded);
            if (f4_range.first == f4_range.second) continue; // Skip if no match in file4

            auto f1_range = file1_map.equal_range(f3_record.key_encoded);
            if (f1_range.first == f1_range.second) continue; // Skip if no match in file1

            auto f2_range = file2_map.equal_range(f3_record.key_encoded);
            if (f2_range.first == f2_range.second) continue; // Skip if no match in file2

            for (auto f4_it = f4_range.first; f4_it != f4_range.second; ++f4_it) {
                for (auto f1_it = f1_range.first; f1_it != f1_range.second; ++f1_it) {
                    for (auto f2_it = f2_range.first; f2_it != f2_range.second; ++f2_it) {
                        std::cout.write(f4_it->second.key, f4_it->second.key_length) << ",";
                        std::cout.write(f1_it->second.key, f1_it->second.key_length) << ",";
                        std::cout.write(f1_it->second.value, f1_it->second.value_length) << ",";
                        std::cout.write(f2_it->second.value, f2_it->second.value_length) << ",";
                        std::cout.write(f4_it->second.value, f4_it->second.value_length)<< "\n";
                    }
                }
            }
        }

        for (int i = 0; i < 4; i++) {
            munmap(const_cast<char*>(data_arr[i]), sbs[i].st_size);
            close(fds[i]);
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
