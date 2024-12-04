#include <iostream>
#include <huffman.hpp>

int main() {
    huffman::Node* root = nullptr;
    std::unordered_map<char, std::string> huffmanCodes;

    // Compress input.txt to compressed.bin
    compressFile("input.txt", "compressed.bin", root, huffmanCodes);

    // Decompress compressed.bin to output.txt
    decompressFile("compressed.bin", "output.txt", root);

    std::cout << "Compression and decompression complete. Check output.txt for results.\n";

    return 0;
}