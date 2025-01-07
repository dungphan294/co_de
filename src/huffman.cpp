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

    std::string decode(const std::string &encoded_text, Node *root)
    {
        std::string decoded_text;
        Node *current = root;

        // Traverse the Huffman tree
        for (char bit : encoded_text)
        {
            if (bit == '0')
            {
                current = current->left;
            }
            else
            {
                current = current->right;
            }

            if (current->left == nullptr && current->right == nullptr)
            {
                decoded_text += current->character;
                current = root;
            }
        }
        return decoded_text;
    }

    // Write ZIP-Like File
    void writeZipFile(const std::string& compressedFile, const std::string& encodedText,
                      const std::unordered_map<char, std::string>& huffmanCodes, const std::string& originalFileName) {
        std::ofstream outFile(compressedFile, std::ios::binary);
        if (!outFile) {
            throw std::runtime_error("Failed to open output file for writing.");
        }

        // Write the original file name length and the file name
        size_t fileNameLength = originalFileName.size();
        outFile.write(reinterpret_cast<const char*>(&fileNameLength), sizeof(fileNameLength));
        outFile.write(originalFileName.c_str(), fileNameLength);

        // Write the Huffman codes to the file
        size_t mapSize = huffmanCodes.size();
        outFile.write(reinterpret_cast<const char*>(&mapSize), sizeof(mapSize));
        for (const auto& pair : huffmanCodes) {
            outFile.put(pair.first);  // Write the character
            size_t codeLength = pair.second.size();
            outFile.write(reinterpret_cast<const char*>(&codeLength), sizeof(codeLength));
            outFile.write(pair.second.c_str(), codeLength);  // Write the Huffman code
        }

        // Write the length of the encoded text
        size_t encodedSize = encodedText.size();
        outFile.write(reinterpret_cast<const char*>(&encodedSize), sizeof(encodedSize));

        // Write the encoded text as binary data (convert every 8 bits to a byte)
        for (size_t i = 0; i < encodedSize; i += 8) {
            std::string byteStr = encodedText.substr(i, 8);
            while (byteStr.size() < 8) {
                byteStr += '0';  // Pad with '0' if the final byte is less than 8 bits
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
} // namespace huffman
