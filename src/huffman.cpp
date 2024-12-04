#include <huffman.hpp>

namespace huffman
{
    Node::Node(char ch, int freq) : frequency(freq),
                                    character(ch),
                                    left(nullptr),
                                    right(nullptr) {}

    bool compare::operator()(Node *left, Node *right)
    {
        return left->frequency > right->frequency;
    }

    Node *Huffman_tree(const std::string &text)
    {
        std::unordered_map<char, int> freq;
        for (char ch : text)
        {
            freq[ch]++;
        }

        // Create a priority queue to build the tree
        std::priority_queue<Node *, std::vector<Node *>, compare> minHeap;
        for (const auto &pair : freq)
        {
            minHeap.push(new Node(pair.first, pair.second));
        }

        // Build the Huffman tree
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

    void compressFile(const std::string &inputFile, const std::string &outputFile, Node *&root, std::unordered_map<char, std::string> &huffmanCodes)
    {
        // Read the input file
        std::ifstream inFile(inputFile);
        std::string text((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>()); // read characters directly from an input stream
        inFile.close();

        // Build Huffman Tree and Codes
        root = Huffman_tree(text);
        print_code(root, "", huffmanCodes);

        // Encode the text
        std::string encodedText = encode(text, huffmanCodes);

        // Write encoded binary data to the output file
        std::ofstream outFile(outputFile, std::ios::binary); // open file in binary mode
        outFile << encodedText;
        outFile.close();
    }

    void decompressFile(const std::string &inputFile, const std::string &outputFile, Node *root)
    {
        // Read the encoded binary file
        std::ifstream inFile(inputFile, std::ios::binary);
        std::string encodedText((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>()); // read characters directly from an input stream
        inFile.close();

        // Decode the text
        std::string decodedText = decode(encodedText, root);

        // Write the decoded text to the output file
        std::ofstream outFile(outputFile);
        outFile << decodedText;
        outFile.close();
    }

} // namespace huffman
