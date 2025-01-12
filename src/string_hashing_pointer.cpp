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

typedef uint64_t encoded_string_t;

struct Record {
    std::string* key;
    std::string* value;
};

std::string* find(std::unordered_multimap<encoded_string_t, std::string>& string_table, const std::string& key) {
    encoded_string_t hashed_key = std::hash<std::string>{}(key);

    auto range = string_table.equal_range(hashed_key);
    if (range.first == range.second) {
        string_table.insert({hashed_key, key});
    }

    return &string_table.find(hashed_key)->second;
}

std::vector<Record> readCSV(const std::string& filename, std::unordered_multimap<encoded_string_t, std::string>& string_table) {
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

    // Memory map the file
    const char* data = static_cast<const char*>(
        mmap(nullptr, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0)
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
                record.key = find(string_table, std::string(line_start, comma - line_start));
                record.value = find(string_table, std::string(comma + 1, current - (comma + 1)));
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
            record.key = find(string_table, std::string(line_start, comma - line_start));
            record.value = find(string_table, std::string(comma + 1, current - (comma + 1)));
            records.push_back(std::move(record));
        }
    }

    // Cleanup
    munmap(const_cast<char*>(data), sb.st_size);
    close(fd);

    return records;
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " file1.csv file2.csv file3.csv file4.csv\n";
        return 1;
    }

    try {
        std::unordered_multimap<encoded_string_t, std::string> string_table;

        // Read all files with buffered I/O
        auto file1 = readCSV(argv[1], string_table);
        auto file2 = readCSV(argv[2], string_table);
        auto file3 = readCSV(argv[3], string_table);
        auto file4 = readCSV(argv[4], string_table);

        // Create hash maps
        std::unordered_multimap<std::string*, std::string*> file1_map;
        std::unordered_multimap<std::string*, std::string*> file2_map;
        std::unordered_multimap<std::string*, std::string*> file4_map;

        // Reserve space in hash maps
        file1_map.reserve(file1.size());
        file2_map.reserve(file2.size());
        file4_map.reserve(file4.size());

        // Build hash tables
        for (const auto& record : file1) {
            file1_map.emplace(record.key, record.value);
        }
        for (const auto& record : file2) {
            file2_map.emplace(record.key, record.value);
        }
        for (const auto& record : file4) {
            file4_map.emplace(record.key, record.value);
        }

        for (const auto& f3_record : file3) {
            auto f4_range = file4_map.equal_range(f3_record.value);
            for (auto f4_it = f4_range.first; f4_it != f4_range.second; ++f4_it) {
                auto f1_range = file1_map.equal_range(f3_record.key);
                for (auto f1_it = f1_range.first; f1_it != f1_range.second; ++f1_it) {
                    auto f2_range = file2_map.equal_range(f3_record.key);
                    for (auto f2_it = f2_range.first; f2_it != f2_range.second; ++f2_it) {
                        std::cout << *(f4_it->first) << ","
                                 << *(f1_it->first) << ","
                                 << *(f1_it->second) << ","
                                 << *(f2_it->second) << ","
                                 << *(f4_it->second) << "\n";
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
