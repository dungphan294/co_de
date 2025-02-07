#include "lzw.hpp"
#include <iostream>
#include <fstream>
#include <stdexcept>

/**
 * @brief Initializes the dictionary with the first 256 single-byte sequences.
 */
void lzw::LZW::initializeDictionary()
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
void lzw::LZW::compress(const std::string &inputFile, const std::string &outputFile)
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
void lzw::LZW::decompress(const std::string &inputFile, const std::string &outputFile)
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
