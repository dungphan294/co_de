#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>

#define DICTIONARY_SIZE 4096

// Function to compress a file using LZW algorithm
void compressFile(const std::string& inputFile, const std::string& outputFile) {
    std::ifstream input(inputFile, std::ios::binary);
    std::ofstream output(outputFile, std::ios::binary);
    
    if (!input.is_open() || !output.is_open()) {
        std::cerr << "Error opening files!" << std::endl;
        return;
    }

    // Initialize dictionary with single characters (0-255)
    std::unordered_map<std::string, int> dictionary;
    for (int i = 0; i < 256; i++) {
        std::string ch = std::string(1, static_cast<char>(i));
        dictionary[ch] = i;
    }

    int nextCode = 256;
    std::string current;
    char c;
    std::vector<int> compressed;

    // Read input file character by character
    while (input.get(c)) {
        std::string next = current + c;
        
        if (dictionary.find(next) != dictionary.end()) {
            current = next;
        } else {
            compressed.push_back(dictionary[current]);
            
            // Add new sequence to dictionary if not full
            if (nextCode < DICTIONARY_SIZE) { // 2^16 limit for this implementation
                dictionary[next] = nextCode++;
            }
            
            current = std::string(1, c);
        }
    }

    // Output last code if necessary
    if (!current.empty()) {
        compressed.push_back(dictionary[current]);
    }

    // Write compressed data to output file
    // First write the size of compressed vector
    size_t size = compressed.size();
    output.write(reinterpret_cast<const char*>(&size), sizeof(size));
    
    // Write the compressed codes
    for (int code : compressed) {
        output.write(reinterpret_cast<const char*>(&code), sizeof(int));
    }

    input.close();
    output.close();
}

// Function to decompress a file using LZW algorithm
void decompressFile(const std::string& inputFile, const std::string& outputFile) {
    std::ifstream input(inputFile, std::ios::binary);
    std::ofstream output(outputFile, std::ios::binary);
    
    if (!input.is_open() || !output.is_open()) {
        std::cerr << "Error opening files!" << std::endl;
        return;
    }

    // Initialize dictionary with single characters (0-255)
    std::vector<std::string> dictionary;
    for (int i = 0; i < 256; i++) {
        dictionary.push_back(std::string(1, static_cast<char>(i)));
    }

    // Read the size of compressed data
    size_t size;
    input.read(reinterpret_cast<char*>(&size), sizeof(size));

    // Read compressed codes
    std::vector<int> compressed;
    for (size_t i = 0; i < size; i++) {
        int code;
        input.read(reinterpret_cast<char*>(&code), sizeof(int));
        compressed.push_back(code);
    }

    // Decompress the data
    std::string current = dictionary[compressed[0]];
    output << current;

    for (size_t i = 1; i < compressed.size(); i++) {
        int code = compressed[i];
        std::string entry;
        
        if (code < int(dictionary.size())) {
            entry = dictionary[code];
        } else {
            entry = current + current[0];
        }
        
        output << entry;
        
        // Add new sequence to dictionary if not full
        if (dictionary.size() < DICTIONARY_SIZE) { // 2^16 limit
            dictionary.push_back(current + entry[0]);
        }
        
        current = entry;
    }

    input.close();
    output.close();
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cout << "Usage: " << argv[0] << " [c/d] input_file output_file" << std::endl;
        std::cout << "c - compress" << std::endl;
        std::cout << "d - decompress" << std::endl;
        return 1;
    }

    std::string mode = argv[1];
    std::string inputFile = argv[2];
    std::string outputFile = argv[3];

    if (mode == "c") {
        compressFile(inputFile, outputFile);
        std::cout << "File compressed successfully." << std::endl;
    } else if (mode == "d") {
        decompressFile(inputFile, outputFile);
        std::cout << "File decompressed successfully." << std::endl;
    } else {
        std::cout << "Invalid mode. Use 'c' for compress or 'd' for decompress." << std::endl;
        return 1;
    }

    return 0;
}