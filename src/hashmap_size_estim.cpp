#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>

struct Record {
    std::string key;
    std::string value;
};

std::vector<Record> readCSV(const std::string& filename) {
    std::vector<Record> records;
    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        Record record;
        std::getline(ss, record.key, ',');
        std::getline(ss, record.value);
        records.push_back(record);
    }

    return records;
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " file1.csv file2.csv file3.csv file4.csv\n";
        return 1;
    }

    // Read all files
    auto file1 = readCSV(argv[1]);
    auto file2 = readCSV(argv[2]);
    auto file3 = readCSV(argv[3]);
    auto file4 = readCSV(argv[4]);

    // Create hash tables for faster lookups
    std::unordered_multimap<std::string, Record> file1_map;
    std::unordered_multimap<std::string, Record> file2_map;
    std::unordered_multimap<std::string, Record> file4_map;

    // Reserve space in hash maps
    file1_map.reserve(file1.size());
    file2_map.reserve(file2.size());
    file4_map.reserve(file4.size());


    // Build hash tables
    for (const auto& record : file1) {
        file1_map.insert({record.key, record});
    }
    for (const auto& record : file2) {
        file2_map.insert({record.key, record});
    }
    for (const auto& record : file4) {
        file4_map.insert({record.key, record});
    }

    // Perform join using hash lookups
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

    return 0;
}
