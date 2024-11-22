#include <iostream>
#include "unordered_map"
#include "vector"
#include "string"
#include "queue"

// Huffman tree node
struct Node{
    int frequency;
    char character;
    Node * left;
    Node * right;

    Node(char ch, int freq) : 
        frequency(freq), 
        character(ch), 
        left(nullptr), 
        right(nullptr) {}
};

// Compare the priority queue
struct compare {
    bool operator()(Node * left, Node * right){
        return left->frequency > right ->frequency;
    }
};

Node * Huffman_tree(const std::string &text){
    std::unordered_map<char, int > freq;
    for (char ch : text){
        freq[ch]++;
    }

    // Create a priority queue to build the tree
    std::priority_queue<Node*, std::vector<Node*>, compare> minHeap;
    for (const auto& pair: freq){
        minHeap.push(new Node(pair.first, pair.second));
    }

    // Build the Huffman tree
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

void print_code(Node* root, const std::string& str, std::unordered_map<char, std::string>& Huffman_tree){
    if(!root) return;
    if(root->character != '\0'){
        Huffman_tree[root->character] = str;
    }

    print_code(root->left, str + "0", Huffman_tree);// Traverse the left subtree by appending "0" to the code.
    print_code(root->right, str + "1", Huffman_tree);// Traverse the right subtree by appending "1" to the code.
}

std::string encode(const std::string &str, const std::unordered_map<char, std::string> & Huffman_tree){
    std::string text;
    for (char ch : str){
        text += Huffman_tree.at(ch);
    }
    return text;
}

std::string decode(const std::string &encoded_text, Node *root){
    std::string decoded_text;
    Node *current = root;

    // Traverse the Huffman tree
    for (char bit : encoded_text) {
        if (bit == '0') {
            current = current->left;
        } else {
            current = current->right;
        }

        if (current->left == nullptr && current->right == nullptr) {
            decoded_text += current->character;
            current = root;
        }
    }
    return decoded_text;
}


int main(){
    // Input text
    std::string text;
    std::cout << "Enter the text to compress: ";
    std::getline(std::cin, text);

    // Build the Huffman tree
    Node* root = Huffman_tree(text);

    // Generate Huffman codes
    std::unordered_map<char, std::string> huffmanCodes;
    print_code(root, "", huffmanCodes);

    // Display the Huffman codes
    std::cout << "\nHuffman Codes:\n";
    for (const auto& pair : huffmanCodes) {
        std::cout << pair.first << ": " << pair.second << '\n';
    }

    // Encode the text
    std::string encodedText = encode(text, huffmanCodes);
    std::cout << "\nEncoded Text: " << encodedText << '\n';

    // Decode the encoded text
    std::string decodedText = decode(encodedText, root);
    std::cout << "Decoded Text: " << decodedText << '\n';

    return 0;
}