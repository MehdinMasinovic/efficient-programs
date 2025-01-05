#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

struct Record {
    std::string key;
    std::string value;
};

std::vector<Record> readCSV(const char* filename) {
    std::vector<Record> records;
    records.reserve(1000000);  // Pre-allocate space

    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        throw std::runtime_error("Cannot open file: " + std::string(filename));
    }

    // 1MB buffer for reading
    char buffer[1024*1024];
    char leftover[1024];  // For lines split across buffer boundaries
    size_t leftover_size = 0;

    ssize_t bytes_read;
    while ((bytes_read = read(fd, buffer + leftover_size, sizeof(buffer) - leftover_size)) > 0) {
        char* start = buffer;
        char* end = buffer + leftover_size + bytes_read;
        char* current = start;

        // Process each line in the buffer
        while (current < end) {
            char* line_end = static_cast<char*>(memchr(current, '\n', end - current));

            if (line_end) {
                // Found a complete line
                char* comma = static_cast<char*>(memchr(current, ',', line_end - current));
                if (comma) {
                    Record record;
                    record.key.assign(current, comma - current);
                    record.value.assign(comma + 1, line_end - (comma + 1));
                    records.push_back(std::move(record));
                }
                current = line_end + 1;
            } else {
                // Incomplete line at buffer end
                leftover_size = end - current;
                memmove(leftover, current, leftover_size);
                break;
            }
        }

        // Copy any leftover data to beginning of buffer
        if (leftover_size > 0) {
            memcpy(buffer, leftover, leftover_size);
        }
    }

    // Handle any final leftover data
    if (leftover_size > 0) {
        char* comma = static_cast<char*>(memchr(leftover, ',', leftover_size));
        if (comma) {
            Record record;
            record.key.assign(leftover, comma - leftover);
            record.value.assign(comma + 1, leftover_size - (comma - leftover) - 1);
            records.push_back(std::move(record));
        }
    }

    close(fd);
    return records;
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " file1.csv file2.csv file3.csv file4.csv\n";
        return 1;
    }

    try {
        // Read all files using custom IO
        auto file1 = readCSV(argv[1]);
        auto file2 = readCSV(argv[2]);
        auto file3 = readCSV(argv[3]);
        auto file4 = readCSV(argv[4]);

        // Create hash maps
        std::unordered_multimap<std::string, Record> file1_map;
        std::unordered_multimap<std::string, Record> file2_map;
        std::unordered_multimap<std::string, Record> file4_map;

        // Reserve space in hash maps
        file1_map.reserve(file1.size());
        file2_map.reserve(file2.size());
        file4_map.reserve(file4.size());

        // Build hash tables
        for (const auto& record : file1) {
            file1_map.emplace(record.key, record);
        }
        for (const auto& record : file2) {
            file2_map.emplace(record.key, record);
        }
        for (const auto& record : file4) {
            file4_map.emplace(record.key, record);
        }

        // Set up output buffering
        std::ios_base::sync_with_stdio(false);
        std::cout.tie(nullptr);
        char output_buffer[1024*1024];  // 1MB output buffer
        std::cout.rdbuf()->pubsetbuf(output_buffer, sizeof(output_buffer));

        // Perform join
        for (const auto& f3_record : file3) {
            auto f4_range = file4_map.equal_range(f3_record.value);
            for (auto f4_it = f4_range.first; f4_it != f4_range.second; ++f4_it) {
                auto f1_range = file1_map.equal_range(f3_record.key);
                for (auto f1_it = f1_range.first; f1_it != f1_range.second; ++f1_it) {
                    auto f2_range = file2_map.equal_range(f3_record.key);
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
