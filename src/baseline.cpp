#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

// Basic structure to hold a record from any file
struct Record {
    std::string key;
    std::string value;
};

// Read a CSV file into a vector of records
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

    // Perform nested loop join
    for (const auto& f3_record : file3) {
        // Find matching records in file4 based on f3's second field
        for (const auto& f4_record : file4) {
            if (f3_record.value == f4_record.key) {
                // For each match, find records in file1 and file2 based on f3's first field
                for (const auto& f1_record : file1) {
                    if (f1_record.key == f3_record.key) {
                        for (const auto& f2_record : file2) {
                            if (f2_record.key == f3_record.key) {
                                // Output in required format:
                                // f4.key, f1.key, f1.value, f2.value, f4.value
                                std::cout << f4_record.key << ","
                                         << f1_record.key << ","
                                         << f1_record.value << ","
                                         << f2_record.value << ","
                                         << f4_record.value << "\n";
                            }
                        }
                    }
                }
            }
        }
    }

    return 0;
}
