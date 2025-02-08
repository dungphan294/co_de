#include <huffman.hpp>

namespace huffman
{
    Node::Node(char ch, int freq) : frequency(freq),
                                    character(ch),
                                    left(nullptr),
                                    right(nullptr) {}

    // Comparison for priority queue
    bool compare::operator()(Node *left, Node *right)
    {
        return left->frequency > right->frequency;
    }

    // Build Huffman Tree
    Node* buildHuffmanTree(const std::string& text) {
        std::unordered_map<char, int> freqMap;
        for (char ch : text) freqMap[ch]++;
        std::priority_queue<Node*, std::vector<Node*>, compare> minHeap;
        for (const auto& pair : freqMap) {
            minHeap.push(new Node(pair.first, pair.second));
        }
        while (minHeap.size() > 1) {
            Node* left = minHeap.top(); minHeap.pop();
            Node* right = minHeap.top(); minHeap.pop();
            Node* mergedNode = new Node('\0', left->frequency + right->frequency);
            mergedNode->left = left;
            mergedNode->right = right;
            minHeap.push(mergedNode);
        }
        return minHeap.top();
    }

    void print_code(Node *root, const std::string &str, std::unordered_map<char, std::string> &Huffman_tree)
    {
        if (!root)
            return;
        if (root->character != '\0')
        {
            Huffman_tree[root->character] = str;
        }

        print_code(root->left, str + "0", Huffman_tree);  // Traverse the left subtree by appending "0" to the code.
        print_code(root->right, str + "1", Huffman_tree); // Traverse the right subtree by appending "1" to the code.
    }

    std::string encode(const std::string &str, const std::unordered_map<char, std::string> &Huffman_tree)
    {
        std::string text;
        for (char ch : str)
        {
            text += Huffman_tree.at(ch);
        }
        return text;
    }

    void compressZipfile(const std::string &inputFile, const std::string &outputFile) {
        std::ifstream inFile(inputFile, std::ios::binary);
        if (!inFile) {
            throw std::runtime_error("Failed to open input file: " + inputFile);
        }

        std::ostringstream buffer;
        buffer << inFile.rdbuf();
        std::string inputText = buffer.str();
        inFile.close();

        Node* root = buildHuffmanTree(inputText);
        std::unordered_map<char, std::string> huffmanCodes;
        print_code(root, "", huffmanCodes);
        std::string encodedText = encode(inputText, huffmanCodes);

        std::ofstream outFile(outputFile, std::ios::binary);
        if (!outFile) {
            throw std::runtime_error("Failed to open output file: " + outputFile);
        }

        size_t fileNameLength = inputFile.size();
        outFile.write(reinterpret_cast<const char*>(&fileNameLength), sizeof(fileNameLength));
        outFile.write(inputFile.c_str(), fileNameLength);

        size_t mapSize = huffmanCodes.size();
        outFile.write(reinterpret_cast<const char*>(&mapSize), sizeof(mapSize));
        for (const auto& pair : huffmanCodes) {
            outFile.put(pair.first);
            size_t codeLength = pair.second.size();
            outFile.write(reinterpret_cast<const char*>(&codeLength), sizeof(codeLength));
            outFile.write(pair.second.c_str(), codeLength);
        }

        size_t encodedSize = encodedText.size();
        outFile.write(reinterpret_cast<const char*>(&encodedSize), sizeof(encodedSize));

        for (size_t i = 0; i < encodedSize; i += 8) {
            std::string byteStr = encodedText.substr(i, 8);
            while (byteStr.size() < 8) {
                byteStr += '0';
            }
            std::bitset<8> byte(byteStr);
            outFile.put(static_cast<unsigned char>(byte.to_ulong()));
        }
        outFile.close();
    }

    void decompressZipFile(const std::string& compressedFile, std::string& decompressedText) {
        std::ifstream inFile(compressedFile, std::ios::binary);
        if (!inFile) {
            throw std::runtime_error("Failed to open compressed file.");
        }

        // Read original file name length and skip it
        size_t fileNameLength;
        inFile.read(reinterpret_cast<char*>(&fileNameLength), sizeof(fileNameLength));
        std::string fileName(fileNameLength, '\0');
        inFile.read(&fileName[0], fileNameLength);

        // Read Huffman code mappings
        size_t mapSize;
        inFile.read(reinterpret_cast<char*>(&mapSize), sizeof(mapSize));
        std::unordered_map<std::string, char> reverseHuffmanCodes;
        for (size_t i = 0; i < mapSize; ++i) {
            char ch = inFile.get();
            size_t codeLength;
            inFile.read(reinterpret_cast<char*>(&codeLength), sizeof(codeLength));
            std::string code(codeLength, '\0');
            inFile.read(&code[0], codeLength);
            reverseHuffmanCodes[code] = ch;
        }

        // Read encoded text length
        size_t encodedSize;
        inFile.read(reinterpret_cast<char*>(&encodedSize), sizeof(encodedSize));

        // Read the encoded binary data
        std::string encodedText;
        for (size_t i = 0; i < encodedSize; i += 8) {
            unsigned char byte = inFile.get();
            std::bitset<8> bits(byte);
            encodedText += bits.to_string();
        }

        // Decode the binary data using the Huffman codes
        std::string currentCode;
        for (char bit : encodedText) {
            currentCode += bit;
            if (reverseHuffmanCodes.count(currentCode)) {
                decompressedText += reverseHuffmanCodes[currentCode];
                currentCode.clear();
            }
        }

        inFile.close();
    }
    // Write ZIP-Like File
    void compressFolder(const std::string &inputFolder, const std::string &outputFile) {
        std::ofstream outFile(outputFile, std::ios::binary);
        if (!outFile) {
            throw std::runtime_error("Failed to open output file: " + outputFile);
        }

        // Write folder metadata
        size_t fileCount = std::distance(fs::directory_iterator(inputFolder), fs::directory_iterator());
        outFile.write(reinterpret_cast<const char*>(&fileCount), sizeof(fileCount));

        for (const auto &entry : fs::directory_iterator(inputFolder)) {
            if (entry.is_regular_file()) {
                std::string fileName = entry.path().filename().string();
                std::ifstream inFile(entry.path(), std::ios::binary);
                if (!inFile) {
                    throw std::runtime_error("Failed to open input file: " + fileName);
                }

                std::ostringstream buffer;
                buffer << inFile.rdbuf();
                std::string fileContent = buffer.str();
                inFile.close();

                Node *root = buildHuffmanTree(fileContent);
                std::unordered_map<char, std::string> huffmanCodes;
                print_code(root, "", huffmanCodes);
                std::string encodedText = encode(fileContent, huffmanCodes);

                size_t nameLength = fileName.size();
                outFile.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
                outFile.write(fileName.c_str(), nameLength);

                size_t mapSize = huffmanCodes.size();
                outFile.write(reinterpret_cast<const char*>(&mapSize), sizeof(mapSize));
                for (const auto &pair : huffmanCodes) {
                    outFile.put(pair.first);
                    size_t codeLength = pair.second.size();
                    outFile.write(reinterpret_cast<const char*>(&codeLength), sizeof(codeLength));
                    outFile.write(pair.second.c_str(), codeLength);
                }

                size_t encodedSize = encodedText.size();
                outFile.write(reinterpret_cast<const char*>(&encodedSize), sizeof(encodedSize));
                for (size_t i = 0; i < encodedSize; i += 8) {
                    std::string byteStr = encodedText.substr(i, 8);
                    while (byteStr.size() < 8) {
                        byteStr += '0';
                    }
                    std::bitset<8> byte(byteStr);
                    outFile.put(static_cast<unsigned char>(byte.to_ulong()));
                }
            }
        }
        outFile.close();
    }

    void decompressFolder(const std::string &inputFile, const std::string &outputFolder) {
        std::ifstream inFile(inputFile, std::ios::binary);
        if (!inFile) {
            throw std::runtime_error("Failed to open compressed file.");
        }

        if (!fs::exists(outputFolder)) {
            fs::create_directories(outputFolder);
        }

        size_t fileCount;
        inFile.read(reinterpret_cast<char*>(&fileCount), sizeof(fileCount));

        for (size_t i = 0; i < fileCount; ++i) {
            size_t nameLength;
            inFile.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
            std::string fileName(nameLength, '\0');
            inFile.read(&fileName[0], nameLength);

            size_t mapSize;
            inFile.read(reinterpret_cast<char*>(&mapSize), sizeof(mapSize));
            std::unordered_map<std::string, char> reverseHuffmanCodes;
            for (size_t j = 0; j < mapSize; ++j) {
                char ch = inFile.get();
                size_t codeLength;
                inFile.read(reinterpret_cast<char*>(&codeLength), sizeof(codeLength));
                std::string code(codeLength, '\0');
                inFile.read(&code[0], codeLength);
                reverseHuffmanCodes[code] = ch;
            }

            size_t encodedSize;
            inFile.read(reinterpret_cast<char*>(&encodedSize), sizeof(encodedSize));

            std::string encodedText;
            for (size_t j = 0; j < encodedSize; j += 8) {
                unsigned char byte = inFile.get();
                std::bitset<8> bits(byte);
                encodedText += bits.to_string();
            }

            std::string decompressedText;
            std::string currentCode;
            for (char bit : encodedText) {
                currentCode += bit;
                if (reverseHuffmanCodes.count(currentCode)) {
                    decompressedText += reverseHuffmanCodes[currentCode];
                    currentCode.clear();
                }
            }

            std::ofstream outFile(outputFolder + "/" + fileName);
            if (!outFile) {
                throw std::runtime_error("Failed to open output file: " + fileName);
            }
            outFile << decompressedText;
            outFile.close();
        }
        inFile.close();
    }
} // namespace huffman