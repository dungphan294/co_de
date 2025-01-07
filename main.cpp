////#include <iostream>
////#include <fstream>
////#include <string>
////#include <unordered_map>
////#include "huffman.hpp"
////
////int main() {
////    try {
////        // Compress the file
////        std::ifstream inFile("input.txt");
////        std::string text((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
////        inFile.close();
////
////        huffman:: Node* root = huffman:: buildHuffmanTree(text);
////        std::unordered_map<char, std::string> huffmanCodes;
////        huffman::print_code(root, "", huffmanCodes);
////
////        std::string encodedText = huffman:: encode(text, huffmanCodes);
////        huffman::writeZipFile("compressed.huff", encodedText, huffmanCodes, "input.txt");
////
////        std::cout << "File compressed into 'compressed.huff'" << std::endl;
////
////        // Decompress the file
////        std::string decompressedText;
////        huffman::decompressZipFile("compressed.huff", decompressedText);
////
////        // Write the decompressed content to a new file
////        std::ofstream outFile("decompressed.txt");
////        outFile << decompressedText;
////        outFile.close();
////
////        std::cout << "File decompressed into 'decompressed.txt'" << std::endl;
////    } catch (const std::exception& e) {
////        std::cerr << "Error: " << e.what() << std::endl;
////    }
////
////    return 0;
////}
//
//#include <bits/stdc++.h>
//#include <fstream>
//
//// Function to perform LZW encoding on a string and return the encoded vector of codes
//std::vector<int> encoding(const std::string& s1) {
//    std::unordered_map<std::string, int> table;
//    for (int i = 0; i <= 255; i++) {
//        std::string ch = "";
//        ch += char(i);
//        table[ch] = i;
//    }
//
//    std::string p = "", c = "";
//    p += s1[0];
//    int code = 256;
//    std::vector<int> output_code;
//
//    for (size_t i = 0; i < s1.length(); i++) {
//        if (i != s1.length() - 1)
//            c += s1[i + 1];
//
//        if (table.find(p + c) != table.end()) {
//            p = p + c;
//        } else {
//            output_code.push_back(table[p]);
//            table[p + c] = code;
//            code++;
//            p = c;
//        }
//        c = "";
//    }
//
//    output_code.push_back(table[p]);
//    return output_code;
//}
//
//// Function to perform LZW decoding given a vector of encoded codes
//std::string decoding(const std::vector<int>& op) {
//    std::unordered_map<int, std::string> table;
//    for (int i = 0; i <= 255; i++) {
//        std::string ch = "";
//        ch += char(i);
//        table[i] = ch;
//    }
//
//    int old = op[0], n;
//    std::string s = table[old];
//    std::string c = "";
//    c += s[0];
//    std::string result = s;
//
//    int count = 256;
//    for (size_t i = 0; i < op.size() - 1; i++) {
//        n = op[i + 1];
//        if (table.find(n) == table.end()) {
//            s = table[old];
//            s = s + c;
//        } else {
//            s = table[n];
//        }
//
//        result += s;
//        c = "";
//        c += s[0];
//        table[count] = table[old] + c;
//        count++;
//        old = n;
//    }
//
//    return result;
//}
//
//// Function to compress a file using LZW and save to a ZIP-like binary file
//void compressFile(const std::string& inputFile, const std::string& compressedFile) {
//    std::ifstream inFile(inputFile, std::ios::binary);
//    if (!inFile) {
//        throw std::runtime_error("Failed to open input file.");
//    }
//
//    std::ostringstream buffer;
//    buffer << inFile.rdbuf();
//    std::string inputText = buffer.str();
//    inFile.close();
//
//    std::vector<int> encodedData = encoding(inputText);
//
//    // Write to compressed file
//    std::ofstream outFile(compressedFile, std::ios::binary);
//    if (!outFile) {
//        throw std::runtime_error("Failed to open compressed file.");
//    }
//
//    size_t dataSize = encodedData.size();
//    outFile.write(reinterpret_cast<const char*>(&dataSize), sizeof(dataSize));
//
//    for (int code : encodedData) {
//        outFile.write(reinterpret_cast<const char*>(&code), sizeof(code));
//    }
//
//    outFile.close();
//    std::cout << "File compressed successfully.\n";
//}
//
//// Function to decompress a ZIP-like binary file and save the original content to an output file
//void decompressFile(const std::string& compressedFile, const std::string& outputFile) {
//    std::ifstream inFile(compressedFile, std::ios::binary);
//    if (!inFile) {
//        throw std::runtime_error("Failed to open compressed file.");
//    }
//
//    size_t dataSize;
//    inFile.read(reinterpret_cast<char*>(&dataSize), sizeof(dataSize));
//
//    std::vector<int> encodedData(dataSize);
//    for (size_t i = 0; i < dataSize; i++) {
//        inFile.read(reinterpret_cast<char*>(&encodedData[i]), sizeof(int));
//    }
//    inFile.close();
//
//    std::string decompressedText = decoding(encodedData);
//
//    std::ofstream outFile(outputFile, std::ios::binary);
//    if (!outFile) {
//        throw std::runtime_error("Failed to open output file.");
//    }
//
//    outFile.write(decompressedText.c_str(), decompressedText.size());
//    outFile.close();
//    std::cout << "File decompressed successfully.\n";
//}
//
//// Main function to demonstrate compression and decompression
//int main() {
//    std::string inputFile = "sample-15s.mp3";
//    std::string compressedFile = "compressed.lzw";
//    std::string outputFile = "decompressed_lzw.mp3";
//
//    try {
//        // Compress the input file
//        compressFile(inputFile, compressedFile);
//
//        // Decompress the file back to a new file
//        decompressFile(compressedFile, outputFile);
//    } catch (const std::exception& e) {
//        std::cerr << e.what() << '\n';
//    }
//
//    return 0;
//}
//
//
// Created by Nguyen Hieu on 1/5/2025.
//

#include <iostream>
#include <fstream>
#include <sstream> // For std::ostringstream
#include <string>
#include <unordered_map>
#include "include/huffman.hpp"

void printUsage() {
    std::cout << "Usage:\n"
              << "  compressor --algorithm huffman --mode compress --input <input_file> --output <output_file>\n"
              << "  compressor --algorithm huffman --mode decompress --input <input_file> --output <output_file>\n";
}

int main(int argc, char* argv[]) {
    if (argc != 9) {
        printUsage();
        return 1;
    }

    std::string algorithm, mode, inputFile, outputFile;

    for (int i = 1; i < argc; i += 2) {
        std::string arg = argv[i];
        if (arg == "--algorithm") {
            algorithm = argv[i + 1];
        } else if (arg == "--mode") {
            mode = argv[i + 1];
        } else if (arg == "--input") {
            inputFile = argv[i + 1];
        } else if (arg == "--output") {
            outputFile = argv[i + 1];
        } else {
            printUsage();
            return 1;
        }
    }

    if (algorithm != "huffman") {
        std::cerr << "Error: Unsupported algorithm. Only 'huffman' is supported.\n";
        return 1;
    }

    try {
        if (mode == "compress") {
            // Read the input file
            std::ifstream inFile(inputFile, std::ios::binary);
            if (!inFile) {
                throw std::runtime_error("Failed to open input file.");
            }
            std::ostringstream buffer;
            buffer << inFile.rdbuf();
            std::string inputText = buffer.str();
            inFile.close();

            // Build Huffman tree and encode
            huffman::Node* root = huffman::buildHuffmanTree(inputText);
            std::unordered_map<char, std::string> huffmanCodes;
            huffman::print_code(root, "", huffmanCodes);
            std::string encodedText = huffman::encode(inputText, huffmanCodes);

            // Write to compressed file
            huffman::writeZipFile(outputFile, encodedText, huffmanCodes, inputFile);
            std::cout << "Compression successful: " << outputFile << std::endl;

        } else if (mode == "decompress") {
            std::string decompressedText;
            huffman::decompressZipFile(inputFile, decompressedText);

            // Write to decompressed file
            std::ofstream outFile(outputFile, std::ios::binary);
            if (!outFile) {
                throw std::runtime_error("Failed to open output file for writing.");
            }
            outFile << decompressedText;
            outFile.close();
            std::cout << "Decompression successful: " << outputFile << std::endl;

        } else {
            std::cerr << "Error: Invalid mode. Use 'compress' or 'decompress'.\n";
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}