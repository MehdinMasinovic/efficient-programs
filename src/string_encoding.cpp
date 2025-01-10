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

/*
 * This is a conversion table to map each character to a numeric value that is
 * later used to encode a whole string of characters.
 *
 * The dataset only contains 37 different characters as values.
 * All numbers 0-9, capital letters A-Z and the NUL character \0.
 *
 * The largest cell has a length of 22 characters.
 *
 * In order to support the small dataset with the lowercase charactes, both
 * uppercase and lowercase characters map to the same number.
*/
std::unordered_map<char, encoded_string_t> conversion_table = {
    {'0', 0},
    {'1', 1},
    {'2', 2},
    {'3', 3},
    {'4', 4},
    {'5', 5},
    {'6', 6},
    {'7', 7},
    {'8', 8},
    {'9', 9},
    {'a', 10},
    {'b', 11},
    {'c', 12},
    {'d', 13},
    {'e', 14},
    {'f', 15},
    {'g', 16},
    {'h', 17},
    {'i', 18},
    {'j', 19},
    {'k', 20},
    {'l', 21},
    {'m', 22},
    {'n', 23},
    {'o', 24},
    {'p', 25},
    {'q', 26},
    {'r', 27},
    {'s', 28},
    {'t', 29},
    {'u', 30},
    {'v', 31},
    {'w', 32},
    {'x', 33},
    {'y', 34},
    {'z', 35},
    {'A', 10},
    {'B', 11},
    {'C', 12},
    {'D', 13},
    {'E', 14},
    {'F', 15},
    {'G', 16},
    {'H', 17},
    {'I', 18},
    {'J', 19},
    {'K', 20},
    {'L', 21},
    {'M', 22},
    {'N', 23},
    {'O', 24},
    {'P', 25},
    {'Q', 26},
    {'R', 27},
    {'S', 28},
    {'T', 29},
    {'U', 30},
    {'V', 31},
    {'W', 32},
    {'X', 33},
    {'Y', 34},
    {'Z', 35},
    {'\0', 36}
};

/**
 * This represents a string encoded as a number.
 *
 * As already stated, it is not possible to fit the largest values that occur in
 * the dataset into a single 64-bit unsigned integer, thus more integers are
 * needed.
 *
 * The problem is fixed by using two consecutive integers.
 *
 * Since we are now dealing with a compound type, we also have to implement
 * custom logic for comparison.
 *
 * In theory, at most two conditionals have to be checked in order to confirm
 * the equivalence of two EncodedString objects. With short circuiting, this
 * might also reduce down to only one check.
 */
struct EncodedString {
    encoded_string_t a;
    encoded_string_t b;

    EncodedString(const char* source, const size_t size) {
        this->a = 0;
        this->b = 0;
        encoded_string_t& current = this->a;
        for (int i = 0; i < size; ++source, ++i) {
            current = i > 13 ? this->b : this->a;
            current = (current << 6) | (conversion_table.at(*source));
        }
    }
 
    EncodedString() = default;
    ~EncodedString() = default;

    friend constexpr bool operator==(const EncodedString& lhs, const EncodedString& rhs);
    friend constexpr bool operator<(const EncodedString& lhs, const EncodedString& rhs);
    friend std::ostream& operator<<(std::ostream& os, const EncodedString& es);
};

constexpr bool operator==(const EncodedString& lhs, const EncodedString& rhs) {
    return lhs.a == rhs.a && lhs.b == rhs.b;
}

constexpr bool operator<(const EncodedString& lhs, const EncodedString& rhs) {
    return lhs.a < rhs.a || (lhs.a == rhs.a && lhs.b < rhs.b);
}

std::ostream& operator<<(std::ostream& os, const EncodedString& es)
{
    os << es.a << es.b;
    return os;
}

/**
 * Hashing Functor for EncodedString
 */
template <>
struct std::hash<EncodedString> {
    std::size_t operator()(const EncodedString& k) const {
        return std::hash<encoded_string_t>()(k.a) + 
        17 * std::hash<encoded_string_t>()(k.b);
    }
};

struct Record {
    EncodedString key_encoded;
    EncodedString value_encoded;
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
                record.key.assign(line_start, comma - line_start);
                record.value.assign(comma + 1, current - (comma + 1));
                record.key_encoded = EncodedString(line_start, comma - line_start);
                record.value_encoded = EncodedString(comma + 1, current - (comma + 1));
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
            record.key_encoded = EncodedString(line_start, comma - line_start);
            record.value_encoded = EncodedString(comma + 1, current - (comma + 1));
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
        // Read all files with buffered I/O
        auto file1 = readCSV(argv[1]);
        auto file2 = readCSV(argv[2]);
        auto file3 = readCSV(argv[3]);
        auto file4 = readCSV(argv[4]);

        // Create hash maps
        std::unordered_multimap<EncodedString, Record> file1_map;
        std::unordered_multimap<EncodedString, Record> file2_map;
        std::unordered_multimap<EncodedString, Record> file4_map;

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
        for (const auto& f3_record : file3) {
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
