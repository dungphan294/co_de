#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <array>

// Use fixed size types for better control over memory
using Code = uint16_t;
using Byte = uint8_t;

class LZWCompressor {
private:
    static constexpr size_t DICTIONARY_SIZE = 4096;
    static constexpr size_t INITIAL_DICT_SIZE = 256;
    
    // Use a struct for dictionary entries to optimize memory layout
    struct DictEntry {
        std::vector<Byte> sequence;
        bool used = false;
    };
    
    // Preallocate dictionary with fixed size
    std::array<DictEntry, DICTIONARY_SIZE> dictionary;
    
    void initializeDictionary() {
        // Initialize first 256 entries for single bytes
        for (size_t i = 0; i < INITIAL_DICT_SIZE; ++i) {
            dictionary[i].sequence = {static_cast<Byte>(i)};
            dictionary[i].used = true;
        }
    }

public:
    // Compress function with buffer-based I/O for better performance
    void compress(const std::string& inputFile, const std::string& outputFile) {
        std::ifstream input(inputFile, std::ios::binary);
        std::ofstream output(outputFile, std::ios::binary);
        
        if (!input || !output) {
            throw std::runtime_error("Error opening files");
        }

        // Read entire file into buffer
        input.seekg(0, std::ios::end);
        size_t fileSize = input.tellg();
        input.seekg(0);
        std::vector<Byte> buffer(fileSize);
        input.read(reinterpret_cast<char*>(buffer.data()), fileSize);

        // Initialize compression
        std::unordered_map<std::string, Code> dict;
        dict.reserve(DICTIONARY_SIZE);  // Prevent rehashing
        for (Code i = 0; i < INITIAL_DICT_SIZE; ++i) {
            dict[std::string(1, static_cast<char>(i))] = i;
        }

        std::vector<Code> compressed;
        compressed.reserve(fileSize);  // Preallocate for worst case
        
        std::string current;
        Code nextCode = INITIAL_DICT_SIZE;

        // Process buffer
        for (Byte b : buffer) {
            std::string next = current + static_cast<char>(b);
            
            auto it = dict.find(next);
            if (it != dict.end()) {
                current = std::move(next);
            } else {
                compressed.push_back(dict[current]);
                if (nextCode < DICTIONARY_SIZE) {
                    dict.emplace(std::move(next), nextCode++);
                }
                current = std::string(1, static_cast<char>(b));
            }
        }

        if (!current.empty()) {
            compressed.push_back(dict[current]);
        }

        // Write compressed data efficiently
        const size_t compressedSize = compressed.size();
        output.write(reinterpret_cast<const char*>(&compressedSize), sizeof(compressedSize));
        output.write(reinterpret_cast<const char*>(compressed.data()), 
                    compressed.size() * sizeof(Code));
    }

    // Decompress function with buffer-based I/O
    void decompress(const std::string& inputFile, const std::string& outputFile) {
        std::ifstream input(inputFile, std::ios::binary);
        std::ofstream output(outputFile, std::ios::binary);
        
        if (!input || !output) {
            throw std::runtime_error("Error opening files");
        }

        initializeDictionary();
        
        // Read compressed data size
        size_t compressedSize;
        input.read(reinterpret_cast<char*>(&compressedSize), sizeof(compressedSize));

        // Read all compressed codes at once
        std::vector<Code> compressed(compressedSize);
        input.read(reinterpret_cast<char*>(compressed.data()), 
                  compressedSize * sizeof(Code));

        // Output buffer for better write performance
        std::vector<Byte> outputBuffer;
        outputBuffer.reserve(compressedSize * 2);  // Estimate size

        Code nextCode = INITIAL_DICT_SIZE;
        
        // Process first code
        const auto& firstSeq = dictionary[compressed[0]].sequence;
        outputBuffer.insert(outputBuffer.end(), firstSeq.begin(), firstSeq.end());
        
        std::vector<Byte> current = firstSeq;

        // Process remaining codes
        for (size_t i = 1; i < compressedSize; ++i) {
            Code code = compressed[i];
            std::vector<Byte> entry;
            
            if (code < nextCode) {
                entry = dictionary[code].sequence;
            } else {
                entry = current;
                entry.push_back(current[0]);
            }
            
            outputBuffer.insert(outputBuffer.end(), entry.begin(), entry.end());
            
            if (nextCode < DICTIONARY_SIZE) {
                dictionary[nextCode].sequence = current;
                dictionary[nextCode].sequence.push_back(entry[0]);
                dictionary[nextCode].used = true;
                ++nextCode;
            }
            
            current = std::move(entry);
        }

        // Write all output at once
        output.write(reinterpret_cast<const char*>(outputBuffer.data()), 
                    outputBuffer.size());
    }
};

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cout << "Usage: " << argv[0] << " [c/d] input_file output_file\n"
                  << "c - compress\n"
                  << "d - decompress" << std::endl;
        return 1;
    }

    try {
        LZWCompressor compressor;
        if (argv[1][0] == 'c') {
            compressor.compress(argv[2], argv[3]);
            std::cout << "File compressed successfully." << std::endl;
        } else if (argv[1][0] == 'd') {
            compressor.decompress(argv[2], argv[3]);
            std::cout << "File decompressed successfully." << std::endl;
        } else {
            std::cerr << "Invalid mode. Use 'c' for compress or 'd' for decompress." << std::endl;
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}