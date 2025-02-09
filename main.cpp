#include <iostream>
#include <string>
#include <unordered_map>
#include <filesystem>
#include <chrono>
#include "lzw.hpp"
#include "huffman.hpp"

namespace fs = std::filesystem;
using namespace std::chrono;

void printUsage()
{
    std::cout << "Usage:\n"
              << "  compressor --algorithm lzw/huffman --mode compress/decompress -i <input_file_or_folder> -o <output_file_or_folder>\n";
}

int main(int argc, char *argv[])
{
    if (argc < 9 || argc > 10)
    {
        printUsage();
        return 1;
    }

    std::string algorithm, mode, inputPath, outputPath;

    for (int i = 1; i < argc; i += 2)
    {
        std::string arg = argv[i];
        if (arg == "--algorithm" || arg == "-a")
        {
            algorithm = argv[i + 1];
        }
        else if (arg == "--mode" || arg == "-m")
        {
            mode = argv[i + 1];
        }
        else if (arg == "--input" || arg == "-i")
        {
            inputPath = argv[i + 1];
        }
        else if (arg == "--output" || arg == "-o")
        {
            outputPath = argv[i + 1];
        }
        else
        {
            printUsage();
            return 1;
        }
    }

    try
    {
        auto start = high_resolution_clock::now();
        lzw::LZW compressor;

        if (algorithm == "lzw")
        {
            if (mode == "compress")
            {
                if (fs::is_directory(inputPath))
                {
                    std::cout << "Compressing folder: " << inputPath << std::endl;
                    compressor.compressFolder(inputPath, outputPath);
                }
                else
                {
                    compressor.compress(inputPath, outputPath);
                    std::cout << "Compression file: " << inputPath << std::endl;
                }
                std::cout << "Compression successful: " << outputPath << std::endl;
            }
            else if (mode == "decompress")
            {
                // Check if the input file has .folder.lzw extension
                if (inputPath.ends_with(".folder.lzw"))
                {
                    // Create output directory if it doesn't exist
                    if (!fs::exists(outputPath))
                    {
                        fs::create_directories(outputPath);
                    }
                    std::cout << "Decompressing folder archive: " << inputPath << std::endl;
                    compressor.decompressFolder(inputPath, outputPath);
                }
                else
                {
                    // Handle single file decompression
                    fs::path outPath(outputPath);
                    // Create parent directories if they don't exist
                    if (outPath.has_parent_path() && !fs::exists(outPath.parent_path()))
                    {
                        fs::create_directories(outPath.parent_path());
                    }
                    std::cout << "Decompressing file: " << inputPath << std::endl;
                    compressor.decompress(inputPath, outputPath);
                }
                std::cout << "Decompression successful: " << outputPath << std::endl;
            }
            else
            {
                std::cerr << "Error: Invalid mode. Use 'compress' or 'decompress'.\n";
                return 1;
            }
        }
        else if (algorithm == "huffman")
        {
            if (mode == "compress")
            {
                if (fs::is_directory(inputPath))
                {
                    std::cout << "Compressing folder: " << inputPath << std::endl;
                    huffman::compressFolder(inputPath, outputPath);
                }
                else
                {
                    huffman::compress(inputPath, outputPath);
                    std::cout << "Compression file: " << outputPath << std::endl;
                }
                std::cout << "Compression successful: " << outputPath << std::endl;
            }
            else if (mode == "decompress")
            {
                // Check if the input file has .folder.huff extension
                if(inputPath.ends_with(".folder.huff"))
                {
                    // Create output directory if it doesn't exist
                    if (!fs::exists(outputPath))
                    {
                        fs::create_directories(outputPath);
                    }
                    std::cout << "Decompressing folder archive: " << inputPath << std::endl;
                    huffman::decompressFolder(inputPath, outputPath);
                }else {
                    // Handle single file decompression
                    fs::path outPath(outputPath);
                    // Create parent directories if they don't exist
                    if (outPath.has_parent_path() && !fs::exists(outPath.parent_path()))
                    {
                        fs::create_directories(outPath.parent_path());
                    }
                    std::cout << "Decompressing file: " << inputPath << std::endl;
                    huffman::decompress(inputPath, outputPath);
                }
                std::cout << "Decompression successful: " << outputPath << std::endl;
            }
            else
            {
                std::cerr << "Error: Invalid mode. Use 'compress' or 'decompress'.\n";
                return 1;
            }
        }
        else
        {
            std::cerr << "Error: Unsupported algorithm. Use 'lzw' or 'huffman'.\n";
            return 1;
        }

        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(stop - start);
        std::cout << "Execution time: " << duration.count() << " ms" << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}