#include <huffman.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <queue>
#include <bitset>
#include <filesystem>
#include <chrono>

namespace huffman
{
    // Node constructor
    Node::Node(char ch, int freq) : frequency(freq), character(ch), left(nullptr), right(nullptr) {}

    // Comparison for priority queue
    bool compare::operator()(Node *left, Node *right)
    {
        return left->frequency > right->frequency;
    }

    // Build Huffman Tree
    Node *buildHuffmanTree(const std::string &text)
    {
        std::unordered_map<char, int> freqMap;
        for (char ch : text)
            freqMap[ch]++;
        std::priority_queue<Node *, std::vector<Node *>, compare> minHeap;
        for (const auto &pair : freqMap)
        {
            minHeap.push(new Node(pair.first, pair.second));
        }
        while (minHeap.size() > 1)
        {
            Node *left = minHeap.top();
            minHeap.pop();
            Node *right = minHeap.top();
            minHeap.pop();
            Node *mergedNode = new Node('\0', left->frequency + right->frequency);
            mergedNode->left = left;
            mergedNode->right = right;
            minHeap.push(mergedNode);
        }
        return minHeap.top();
    }

    void deleteHuffmanTree(Node *root)
    {
        if (!root)
            return;
        deleteHuffmanTree(root->left);  // Delete left subtree
        deleteHuffmanTree(root->right); // Delete right subtree
        delete root;                    // Delete the current node
    }

    // Generate Huffman codes
    void print_code(Node *root, const std::string &str, std::unordered_map<char, std::string> &Huffman_tree)
    {
        if (!root)
            return;
        if (root->character != '\0')
        {
            Huffman_tree[root->character] = str;
        }
        print_code(root->left, str + "0", Huffman_tree);  // Traverse left
        print_code(root->right, str + "1", Huffman_tree); // Traverse right
    }

    // Encode text using Huffman codes
    std::string encode(const std::string &str, const std::unordered_map<char, std::string> &Huffman_tree)
    {
        std::string text;
        for (char ch : str)
        {
            text += Huffman_tree.at(ch);
        }
        return text;
    }

    // Compress a single file
    void compress(const std::string &inputFile, const std::string &outputFile)
    {
        std::ifstream inFile(inputFile, std::ios::binary);
        if (!inFile)
        {
            throw std::runtime_error("Failed to open input file: " + inputFile);
        }

        // Read input file into a string
        std::ostringstream buffer;
        buffer << inFile.rdbuf();
        std::string inputText = buffer.str();
        inFile.close();

        // Build Huffman tree and generate codes
        Node *root = buildHuffmanTree(inputText);
        std::unordered_map<char, std::string> huffmanCodes;
        print_code(root, "", huffmanCodes);
        std::string encodedText = encode(inputText, huffmanCodes);

        // Write compressed data to output file
        std::ofstream outFile(outputFile, std::ios::binary);
        if (!outFile)
        {
            throw std::runtime_error("Failed to open output file: " + outputFile);
        }

        // Write Huffman codes to the output file
        size_t mapSize = huffmanCodes.size();
        outFile.write(reinterpret_cast<const char *>(&mapSize), sizeof(mapSize));
        for (const auto &pair : huffmanCodes)
        {
            outFile.put(pair.first);
            size_t codeLength = pair.second.size();
            outFile.write(reinterpret_cast<const char *>(&codeLength), sizeof(codeLength));
            outFile.write(pair.second.c_str(), codeLength);
        }

        // Write encoded text to the output file
        size_t encodedSize = encodedText.size();
        outFile.write(reinterpret_cast<const char *>(&encodedSize), sizeof(encodedSize));

        for (size_t i = 0; i < encodedSize; i += 8)
        {
            std::string byteStr = encodedText.substr(i, 8);
            while (byteStr.size() < 8)
            {
                byteStr += '0'; // Pad with zeros if necessary
            }
            char byte = static_cast<char>(std::bitset<8>(byteStr).to_ulong());
            outFile.put(byte);
        }

        outFile.close();

        // Delete the Huffman tree to free memory
        deleteHuffmanTree(root);
    }

    // Decompress a single file
    void decompress(const std::string &inputFile, const std::string &outputFile)
    {
        std::ifstream inFile(inputFile, std::ios::binary);
        if (!inFile)
        {
            throw std::runtime_error("Failed to open compressed file: " + inputFile);
        }

        // Read Huffman codes from the input file
        size_t mapSize;
        inFile.read(reinterpret_cast<char *>(&mapSize), sizeof(mapSize));
        std::unordered_map<std::string, char> reverseHuffmanCodes;
        for (size_t i = 0; i < mapSize; ++i)
        {
            char ch = inFile.get();
            size_t codeLength;
            inFile.read(reinterpret_cast<char *>(&codeLength), sizeof(codeLength));
            std::string code(codeLength, '\0');
            inFile.read(&code[0], codeLength);
            reverseHuffmanCodes[code] = ch;
        }

        // Read encoded text length
        size_t encodedSize;
        inFile.read(reinterpret_cast<char *>(&encodedSize), sizeof(encodedSize));

        // Read encoded text
        std::string encodedText;
        for (size_t i = 0; i < encodedSize; i += 8)
        {
            char byte;
            inFile.get(byte);
            encodedText += std::bitset<8>(byte).to_string();
        }

        // Decode the text
        std::string currentCode;
        std::string decompressedText;
        for (char bit : encodedText)
        {
            currentCode += bit;
            if (reverseHuffmanCodes.count(currentCode))
            {
                decompressedText += reverseHuffmanCodes[currentCode];
                currentCode.clear();
            }
        }

        // Write decompressed text to the output file
        std::ofstream outFile(outputFile, std::ios::binary);
        if (!outFile)
        {
            throw std::runtime_error("Failed to open output file: " + outputFile);
        }
        outFile << decompressedText;
        outFile.close();
    }

    // Compress a folder
    void compressFolder(const std::string &inputFolder, const std::string &outputFile)
    {
        if (!fs::exists(inputFolder))
        {
            throw std::runtime_error("Input folder does not exist: " + inputFolder);
        }

        // Append .folder.lzw extension if not already present
        std::string finalOutputFile = outputFile;
        if (fs::path(finalOutputFile).extension() != ".folder.huff")
        {
            finalOutputFile += ".folder.huff";
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

        constexpr size_t BUFFER_SIZE = 8192; // 8KB buffer
        std::vector<char> readBuffer(BUFFER_SIZE);

        // Get base path for relative path calculation
        const fs::path basePath = fs::canonical(inputFolder);

        for (const auto &filePath : files)
        {
            const std::string tempCompressedFile = fs::temp_directory_path() / (std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + ".lzw");

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

    // Decompress a folder
    void decompressFolder(const std::string &inputFile, const std::string &outputFolder)
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
            const std::string tempCompressedFile = fs::temp_directory_path() / (std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + ".huff");

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
} // namespace huffman