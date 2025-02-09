#include "lzw.hpp"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <chrono>

namespace lzw
{

    /**
     * @brief Initializes the dictionary with the first 256 single-byte sequences.
     */
    void LZW::initializeDictionary()
    {
        for (size_t i = 0; i < INITIAL_DICT_SIZE; i++)
        {
            dictionary[i].sequence = {static_cast<uint8_t>(i)};
            dictionary[i].used = true;
        }
    }

    /**
     * @brief Compresses a file using the LZW algorithm.
     *
     * Reads the input file, applies the LZW compression algorithm, and writes the compressed data to the output file.
     *
     * @param inputFile Path to the input file to be compressed.
     * @param outputFile Path to the output file where compressed data will be written.
     *
     * @throws std::runtime_error If an error occurs during file operations.
     */
    void LZW::compress(const std::string &inputFile, const std::string &outputFile)
    {
        std::ifstream input(inputFile, std::ios::binary);
        std::ofstream output(outputFile, std::ios::binary);

        if (!input || !output)
        {
            throw std::runtime_error("Error opening files");
        }

        initializeDictionary();

        // Read input file into a buffer
        input.seekg(0, std::ios::end);                                 // Move to the end of the file
        size_t fileSize = input.tellg();                               // Get the file size
        input.seekg(0);                                                // Move back to the beginning of the file
        std::vector<uint8_t> buffer(fileSize);                         // Create a buffer to hold the file contents
        input.read(reinterpret_cast<char *>(buffer.data()), fileSize); // Read the file into the buffer

        // Initialize compression dictionary
        std::unordered_map<std::string, uint16_t> dict; // Dictionary to store sequences
        dict.reserve(DICTIONARY_SIZE);                  // Reserve space for the dictionary to prevent rehashing
        for (uint16_t i = 0; i < INITIAL_DICT_SIZE; ++i)
        {
            dict[std::string(1, static_cast<char>(i))] = i; // Initialize dictionary with single-byte sequences
        }

        std::vector<uint16_t> compressed; // Vector to store compressed data
        compressed.reserve(fileSize);     // Reserve space for the worst-case scenario

        std::string current;                   // Current sequence
        uint16_t nextCode = INITIAL_DICT_SIZE; // Next available code

        // Process each byte in the input buffer
        for (uint8_t byte : buffer)
        {
            std::string next = current + static_cast<char>(byte); // Append the byte to the current sequence
            auto sequene = dict.find(next);                       // Find the sequence in the dictionary

            if (sequene != dict.end())
            {
                current = std::move(next); // Move to the next sequence
            }
            else
            {
                compressed.push_back(dict[current]); // Add the current sequence to the compressed data
                if (nextCode < DICTIONARY_SIZE)
                {
                    dict.emplace(std::move(next), nextCode++); // Add the new sequence to the dictionary
                }
                current = std::string(1, static_cast<char>(byte)); // Start a new sequence with the current byte
            }
        }

        if (!current.empty())
        {
            compressed.push_back(dict[current]); // Add the last sequence to the compressed data
        }

        // Write compressed data to the output file
        const size_t compressedSize = compressed.size();
        output.write(reinterpret_cast<const char *>(&compressedSize), sizeof(compressedSize));
        output.write(reinterpret_cast<const char *>(compressed.data()), compressed.size() * sizeof(uint16_t));
    }

    /**
     * @brief Decompresses a previously compressed file using the LZW algorithm.
     *
     * Reads the compressed file, applies the LZW decompression algorithm, and writes the decompressed data to the output file.
     *
     * @param inputFile Path to the compressed input file.
     * @param outputFile Path to the output file where decompressed data will be written.
     *
     * @throws std::runtime_error If an error occurs during file operations.
     */
    void LZW::decompress(const std::string &inputFile, const std::string &outputFile)
    {
        std::ifstream input(inputFile, std::ios::binary);
        std::ofstream output(outputFile, std::ios::binary);

        if (!input || !output)
        {
            throw std::runtime_error("Error opening files");
        }

        initializeDictionary();

        // Read compressed data size
        size_t compressedSize;
        input.read(reinterpret_cast<char *>(&compressedSize), sizeof(compressedSize));

        std::vector<uint16_t> compressed(compressedSize);
        input.read(reinterpret_cast<char *>(compressed.data()), compressedSize * sizeof(uint16_t));

        std::vector<uint8_t> outputBuffer;
        outputBuffer.reserve(compressedSize * 2);

        uint16_t nextCode = INITIAL_DICT_SIZE;

        // Process first code
        const auto &firstSeq = dictionary[compressed[0]].sequence;
        outputBuffer.insert(outputBuffer.end(), firstSeq.begin(), firstSeq.end());

        std::vector<uint8_t> current = firstSeq;

        // Process remaining codes
        for (size_t i = 1; i < compressedSize; ++i)
        {
            uint16_t code = compressed[i];
            std::vector<uint8_t> entry;

            if (code < nextCode)
            {
                entry = dictionary[code].sequence;
            }
            else
            {
                entry = current;
                entry.push_back(current[0]);
            }

            outputBuffer.insert(outputBuffer.end(), entry.begin(), entry.end());

            if (nextCode < DICTIONARY_SIZE)
            {
                dictionary[nextCode].sequence = current;
                dictionary[nextCode].sequence.push_back(entry[0]);
                dictionary[nextCode].used = true;
                ++nextCode;
            }

            current = std::move(entry);
        }

        output.write(reinterpret_cast<const char *>(outputBuffer.data()), outputBuffer.size());
    }

    /**
     * @brief Compresses a folder using the LZW algorithm.
     *
     * Reads the input folder, applies the LZW compression algorithm to each file, and writes the compressed data to the output file.
     *
     * @param inputFolder Path to the input folder to be compressed.
     * @param outputFile Path to the output file where compressed data will be written.
     *
     * @throws std::runtime_error If an error occurs during file operations.
     */
    void LZW::compressFolder(const std::string &inputFolder, const std::string &outputFile)
    {
        if (!fs::exists(inputFolder))
        {
            throw std::runtime_error("Input folder does not exist: " + inputFolder);
        }

        // Append .folder extension if not already present
        std::string finalOutputFile = outputFile;
        if (fs::path(finalOutputFile).extension() != ".folder.lzw")
        {
            finalOutputFile += ".folder.lzw";
        }

        std::ofstream outFile(finalOutputFile, std::ios::binary);
        if (!outFile)
        {
            throw std::runtime_error("Failed to open output file: " + finalOutputFile);
        }

        // Collect all files including those in subfolders
        std::vector<fs::path> files;
        for (const auto &entry : fs::recursive_directory_iterator(inputFolder))
        {
            if (entry.is_regular_file())
            {
                files.push_back(entry.path());
            }
        }

        // Write file count
        const size_t fileCount = files.size();
        outFile.write(reinterpret_cast<const char *>(&fileCount), sizeof(fileCount));

        const std::string fileExtension = fs::path(finalOutputFile).extension().empty() ? ".lzw" : fs::path(finalOutputFile).extension().string();
        constexpr size_t BUFFER_SIZE = 8192; // 8KB buffer
        std::vector<char> readBuffer(BUFFER_SIZE);

        // Get base path for relative path calculation
        const fs::path basePath = fs::canonical(inputFolder);

        for (const auto &filePath : files)
        {
            const std::string tempCompressedFile = fs::temp_directory_path() / (std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + fileExtension);

            try
            {
                // Calculate relative path from input folder
                fs::path relativePath = fs::canonical(filePath).lexically_relative(basePath);
                const std::string relativePathStr = relativePath.string();

                // Compress the file
                compress(filePath.string(), tempCompressedFile);

                // Read compressed file efficiently
                std::ifstream tempFile(tempCompressedFile, std::ios::binary | std::ios::ate);
                if (!tempFile)
                {
                    throw std::runtime_error("Failed to open temp compressed file: " + tempCompressedFile);
                }

                const size_t dataSize = tempFile.tellg();
                tempFile.seekg(0);

                // Write relative path length and path
                const size_t pathLength = relativePathStr.size();
                outFile.write(reinterpret_cast<const char *>(&pathLength), sizeof(pathLength));
                outFile.write(relativePathStr.c_str(), pathLength);

                // Write data size and content
                outFile.write(reinterpret_cast<const char *>(&dataSize), sizeof(dataSize));

                while (tempFile)
                {
                    tempFile.read(readBuffer.data(), BUFFER_SIZE);
                    outFile.write(readBuffer.data(), tempFile.gcount());
                }

                tempFile.close();
                fs::remove(tempCompressedFile);
            }
            catch (const std::exception &e)
            {
                if (fs::exists(tempCompressedFile))
                {
                    fs::remove(tempCompressedFile);
                }
                throw std::runtime_error("Error processing file " + filePath.string() + ": " + e.what());
            }
        }
    }

    /**
     * @brief Decompresses a previously compressed folder using the LZW algorithm.
     *
     * Reads the compressed file, applies the LZW decompression algorithm to each file, and writes the decompressed data to the output folder.
     *
     * @param inputFile Path to the compressed input file.
     * @param outputFolder Path to the output folder where decompressed data will be written.
     *
     * @throws std::runtime_error If an error occurs during file operations.
     */
    void LZW::decompressFolder(const std::string &inputFile, const std::string &outputFolder)
    {
        if (!fs::exists(inputFile))
        {
            throw std::runtime_error("Input file does not exist: " + inputFile);
        }

        std::ifstream inFile(inputFile, std::ios::binary);
        if (!inFile)
        {
            throw std::runtime_error("Failed to open compressed file: " + inputFile);
        }

        // Remove output folder if it exists and create it fresh
        if (fs::exists(outputFolder))
        {
            fs::remove_all(outputFolder);
        }

        // Create fresh output directory
        if (!fs::create_directory(outputFolder))
        {
            throw std::runtime_error("Failed to create output directory: " + outputFolder);
        }

        const std::string fileExtension = fs::path(inputFile).extension().empty() ? ".lzw" : fs::path(inputFile).extension().string();

        size_t fileCount;
        inFile.read(reinterpret_cast<char *>(&fileCount), sizeof(fileCount));

        constexpr size_t BUFFER_SIZE = 8192; // 8KB buffer
        std::vector<char> buffer(BUFFER_SIZE);

        for (size_t i = 0; i < fileCount; ++i)
        {
            // Read relative path
            size_t pathLength;
            inFile.read(reinterpret_cast<char *>(&pathLength), sizeof(pathLength));
            std::string relativePath(pathLength, '\0');
            inFile.read(&relativePath[0], pathLength);

            // Read file size
            size_t dataSize;
            inFile.read(reinterpret_cast<char *>(&dataSize), sizeof(dataSize));

            // Create temporary file
            const std::string tempCompressedFile = fs::temp_directory_path() / (std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + fileExtension);

            try
            {
                // Construct full output path
                fs::path fullOutputPath = fs::path(outputFolder) / relativePath;

                // Create parent directories if they don't exist
                fs::create_directories(fullOutputPath.parent_path());

                // Write compressed data to temporary file
                std::ofstream tempFile(tempCompressedFile, std::ios::binary);
                if (!tempFile)
                {
                    throw std::runtime_error("Failed to create temporary file: " + tempCompressedFile);
                }

                // Copy data in chunks
                size_t remainingBytes = dataSize;
                while (remainingBytes > 0)
                {
                    const size_t bytesToRead = std::min(BUFFER_SIZE, remainingBytes);
                    inFile.read(buffer.data(), bytesToRead);
                    tempFile.write(buffer.data(), bytesToRead);
                    remainingBytes -= bytesToRead;
                }
                tempFile.close();

                // Decompress to final destination
                decompress(tempCompressedFile, fullOutputPath.string());

                // Cleanup
                fs::remove(tempCompressedFile);
            }
            catch (const std::exception &e)
            {
                if (fs::exists(tempCompressedFile))
                {
                    fs::remove(tempCompressedFile);
                }
                throw std::runtime_error("Error processing file " + relativePath + ": " + e.what());
            }
        }
    }
}
