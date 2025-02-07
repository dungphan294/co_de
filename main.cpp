#include <iostream>
#include "lzw.hpp"

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cout << "Usage: " << argv[0] << " [c/d] input_file output_file\n"
                  << "c - compress\n"
                  << "d - decompress" << std::endl;
        return 1;
    }

    try {
        lzw::LZW compressor;
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