#ifndef LZW_H
#define LZW_H

#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <array>
#include <stdexcept>
#include <filesystem>

namespace fs = std::filesystem;

namespace lzw
{
    /**
     * @brief LZW class for compressing and decompressing files using Lempel-Ziv-Welch algorithm.
     *
     * This class provides methods for compressing and decompressing files using the LZW algorithm.
     * The compression and decompression use a dictionary to store sequences of bytes, with the goal of reducing
     * the size of the input data.
     */
    class LZW
    {
    public:
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
        void compress(const std::string &inputFile, const std::string &outputFile);

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
        void decompress(const std::string &inputFile, const std::string &outputFile);

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
        void compressFolder(const std::string &inputFolder, const std::string &outputFile);

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
        void decompressFolder(const std::string &inputFile, const std::string &outputFolder);

    private:
        static constexpr size_t DICTIONARY_SIZE = 4096;  // Maximum size of the dictionary.
        static constexpr size_t INITIAL_DICT_SIZE = 256; // Initial size of the dictionary (first 256 byte entries).

        /**
         * @brief Dictionary entry structure.
         *
         * Holds a sequence of bytes and a flag to track if the entry has been used.
         */
        struct DictEntry
        {
            std::vector<uint8_t> sequence; // Sequence of bytes in the dictionary entry.
            bool used = false;             // Flag indicating whether the entry has been used.
        };

        std::array<DictEntry, DICTIONARY_SIZE> dictionary; // Fixed-size dictionary for compression and decompression.

        /**
         * @brief Initializes the dictionary with the first 256 single-byte sequences.
         */
        void initializeDictionary();
    };
}; // namespace lzw
#endif // LZW_H
