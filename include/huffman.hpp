#ifndef HUFFMAN_TREE_HPP
#define HUFFMAN_TREE_HPP

#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <queue>
#include <bitset>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;
namespace huffman
{
    /**
     * @brief A node in the Huffman tree.
     */
    struct Node
    {
        int frequency;  ///< Frequency of the character.
        char character; ///< Character associated with the node ('\0' for internal nodes).
        Node *left;     ///< Pointer to the left child node.
        Node *right;    ///< Pointer to the right child node.

        /**
         * @brief Constructor for Node.
         * @param ch The character.
         * @param freq The frequency of the character.
         */
        Node(char ch, int freq);
    };

    /**
     * @brief Comparator for the priority queue used in building the Huffman tree.
     */
    struct compare
    {
        /**
         * @brief Compare two nodes by their frequency.
         * @param left Pointer to the left node.
         * @param right Pointer to the right node.
         * @return True if left's frequency is greater than right's.
         */
        bool operator()(Node *left, Node *right);
    };

    /**
     * @brief Build the Huffman tree for a given string.
     * @param text The input string.
     * @return Pointer to the root of the Huffman tree.
     */
    Node *buildHuffmanTree(const std::string &text);

    /**
     * @brief Delete the Huffman tree.
     * @param root Pointer to the root of the Huffman tree.
     */
    void deleteHuffmanTree(Node *root);

    /**
     * @brief Generate Huffman codes for each character.
     * @param root Pointer to the root of the Huffman tree.
     * @param str Current Huffman code string.
     * @param Huffman_tree Map to store the Huffman codes.
     */
    void print_code(Node *root, const std::string &str, std::unordered_map<char, std::string> &Huffman_tree);

    /**
     * @brief Encode a string using Huffman codes.
     * @param str The input string to encode.
     * @param Huffman_tree Map of Huffman codes for characters.
     * @return Encoded string in binary form.
     */
    std::string encode(const std::string &str, const std::unordered_map<char, std::string> &Huffman_tree);

    /**
     * @brief Compresses a file using the Huffman algorithm.
     *
     * Reads the input file, applies the Huffman compression algorithm, and writes the compressed data to the output file.
     *
     * @param inputFile Path to the input file to be compressed.
     * @param outputFile Path to the output file where compressed data will be written.
     */
    void compress(const std::string &inputFile, const std::string &outputFile);

    /**
     * @brief Decompresses a previously compressed file using the Huffman algorithm.
     *
     * Reads the compressed file, applies the Huffman decompression algorithm, and writes the decompressed data to the output file.
     *
     * @param inputFile Path to the compressed input file.
     * @param outputFile Path to the output file where decompressed data will be written.
     */
    void decompress(const std::string &inputFile, const std::string &outputFile);

    /**
     * @brief Compresses a folder using the Huffman algorithm.
     *
     * Reads the input folder, applies the Huffman compression algorithm to each file, and writes the compressed data to the output file.
     *
     * @param inputFolder Path to the input folder to be compressed.
     * @param outputFile Path to the output file where compressed data will be written.
     *
     * @throws std::runtime_error If an error occurs during file operations.
     */
    void compressFolder(const std::string &folderPath, const std::string &outputFile);

    /**
     * @brief Decompresses a previously compressed folder using the Huffman algorithm.
     *
     * Reads the compressed file, applies the Huffman decompression algorithm to each file, and writes the decompressed data to the output folder.
     *
     * @param inputFile Path to the compressed input file.
     * @param outputFolder Path to the output folder where decompressed data will be written.
     *
     * @throws std::runtime_error If an error occurs during file operations.
     */
    void decompressFolder(const std::string &inputFile, const std::string &outputFolder);
}

#endif // HUFFMAN_TREE_HPP