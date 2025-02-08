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
        int frequency;      ///< Frequency of the character.
        char character;     ///< Character associated with the node ('\0' for internal nodes).
        Node *left;         ///< Pointer to the left child node.
        Node *right;        ///< Pointer to the right child node.

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
    Node* buildHuffmanTree(const std::string& text);

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

    void compressZipfile(const std::string &inputFile, const std::string &outputFile);
    void decompressZipFile(std::ifstream& inFile, std::string& decompressedText);

    void compressFolder(const std::string& folderPath, const std::string& outputFile);
    void decompressFolder(const std::string &inputFolder, const std::string &outputFolder);
}

#endif // HUFFMAN_TREE_HPP