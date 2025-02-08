#include <iostream>
#include <string>
#include <unordered_map>
#include "huffman.hpp"
#include <filesystem>

namespace fs = std::filesystem;

void printUsage() {
    std::cout << "Usage:\n"
              << "  compressor --algorithm huffman --mode compress --input <input_file_or_folder> --output <output_file>\n"
              << "  compressor --algorithm huffman --mode decompress --input <input_file> --output <output_folder>\n";
}

int main(int argc, char* argv[]) {
    if (argc != 9) {
        printUsage();
        return 1;
    }

    std::string algorithm, mode, inputPath, outputPath;

    for (int i = 1; i < argc; i += 2) {
        std::string arg = argv[i];
        if (arg == "--algorithm") {
            algorithm = argv[i + 1];
        } else if (arg == "--mode") {
            mode = argv[i + 1];
        } else if (arg == "--input") {
            inputPath = argv[i + 1];
        } else if (arg == "--output") {
            outputPath = argv[i + 1];
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
            if (fs::is_directory(inputPath)) {
                std::cout << "Compressing folder: " << inputPath << std::endl;
                huffman::compressFolder(inputPath, outputPath);
            } else {
                huffman::compressZipfile(inputPath, outputPath);
                std::cout << "Compression file: " << outputPath << std::endl;
            }
            std::cout << "Compression successful: " << outputPath << std::endl;
        } else if (mode == "decompress") {
            if (!fs::is_directory(outputPath)) {
                fs::create_directories(outputPath);
            }
            huffman::decompressFolder(inputPath, outputPath);
            std::cout << "Decompression successful: " << outputPath << std::endl;
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
