#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <map>
#include <memory>

// --- Huffman Tree Node ---
// Represents a node in the Huffman Tree.
// It can be a leaf node (with a character) or an internal node.
struct HuffmanNode {
    char data;
    unsigned frequency;
    std::shared_ptr<HuffmanNode> left, right;

    HuffmanNode(char data, unsigned frequency) : data(data), frequency(frequency), left(nullptr), right(nullptr) {}
};

// --- Comparison Functor for Priority Queue ---
// A comparison structure to order nodes in the min-priority queue.
// The node with the lowest frequency has the highest priority.
struct CompareNodes {
    bool operator()(const std::shared_ptr<HuffmanNode>& l, const std::shared_ptr<HuffmanNode>& r) const {
        return l->frequency > r->frequency;
    }
};

// --- Huffman Coding Class ---
// Encapsulates the entire compression and decompression logic.
class HuffmanCoding {
public:
    // Main function to compress a file.
    void compress(const std::string& inputFilePath, const std::string& outputFilePath);

    // Main function to decompress a file.
    void decompress(const std::string& inputFilePath, const std::string& outputFilePath);

private:
    std::map<char, unsigned> frequencyMap;
    std::map<char, std::string> huffmanCodes;
    std::shared_ptr<HuffmanNode> treeRoot;

    // Helper methods for compression
    void buildFrequencyTable(std::ifstream& inputFile);
    void buildHuffmanTree();
    void generateCodes(const std::shared_ptr<HuffmanNode>& node, const std::string& code);
    void writeHeader(std::ofstream& outputFile);
    void writeCompressedData(std::ifstream& inputFile, std::ofstream& outputFile);

    // Helper methods for decompression
    void readHeader(std::ifstream& inputFile);
    void decodeData(std::ifstream& inputFile, std::ofstream& outputFile);
};

// --- Compression Implementation ---

void HuffmanCoding::buildFrequencyTable(std::ifstream& inputFile) {
    char character;
    frequencyMap.clear();
    // Reset stream to the beginning to read from the start.
    inputFile.clear();
    inputFile.seekg(0, std::ios::beg);
    
    while (inputFile.get(character)) {
        frequencyMap[character]++;
    }
}

void HuffmanCoding::buildHuffmanTree() {
    std::priority_queue<std::shared_ptr<HuffmanNode>, std::vector<std::shared_ptr<HuffmanNode>>, CompareNodes> pq;

    // Create a leaf node for each character and add it to the priority queue.
    for (auto const& [key, val] : frequencyMap) {
        pq.push(std::make_shared<HuffmanNode>(key, val));
    }

    // Edge Case: If the file is empty, the priority queue will be empty.
    if (pq.empty()) {
        treeRoot = nullptr;
        return;
    }

    // Edge Case: If there is only one unique character in the file.
    // The tree must have at least two levels to be traversable.
    // We create a dummy parent node to ensure codes can be generated and decoded.
    if (pq.size() == 1) {
        auto singleNode = pq.top();
        pq.pop();
        auto dummyParent = std::make_shared<HuffmanNode>('$', singleNode->frequency);
        dummyParent->left = singleNode;
        pq.push(dummyParent);
    }

    // Main loop to build the tree.
    // Continues until only one node remains in the queue, which is the root.
    while (pq.size() > 1) {
        // Extract the two nodes with the lowest frequency.
        auto left = pq.top();
        pq.pop();
        auto right = pq.top();
        pq.pop();

        // Create a new internal node with these two nodes as children.
        // The frequency is the sum of the children's frequencies.
        // A special character '$' is used for internal nodes (data is irrelevant).
        auto newNode = std::make_shared<HuffmanNode>('$', left->frequency + right->frequency);
        newNode->left = left;
        newNode->right = right;
        pq.push(newNode);
    }
    
    // The remaining node is the root of the Huffman Tree.
    treeRoot = pq.top();
}


void HuffmanCoding::generateCodes(const std::shared_ptr<HuffmanNode>& node, const std::string& code) {
    if (!node) {
        return;
    }
    // If it's a leaf node (has a character), store the generated code.
    if (!node->left && !node->right) {
        huffmanCodes[node->data] = code;
    }
    // Recursively traverse left (append '0') and right (append '1').
    generateCodes(node->left, code + "0");
    generateCodes(node->right, code + "1");
}

void HuffmanCoding::writeHeader(std::ofstream& outputFile) {
    // The header contains the frequency map, which is needed for decompression.
    size_t mapSize = frequencyMap.size();
    outputFile.write(reinterpret_cast<const char*>(&mapSize), sizeof(mapSize));

    // Write the character-frequency pairs.
    for (const auto& pair : frequencyMap) {
        outputFile.write(&pair.first, sizeof(pair.first));
        outputFile.write(reinterpret_cast<const char*>(&pair.second), sizeof(pair.second));
    }
}

void HuffmanCoding::writeCompressedData(std::ifstream& inputFile, std::ofstream& outputFile) {
    // Go back to the beginning of the input file to read its contents.
    inputFile.clear();
    inputFile.seekg(0, std::ios::beg);

    char character;
    unsigned char buffer = 0;
    int bitCount = 0;

    while (inputFile.get(character)) {
        std::string code = huffmanCodes[character];
        for (char bit : code) {
            // Add the bit to the buffer by left-shifting and ORing.
            buffer = (buffer << 1) | (bit - '0');
            bitCount++;
            // If the buffer is full (8 bits), write it to the file.
            if (bitCount == 8) {
                outputFile.put(buffer);
                buffer = 0;
                bitCount = 0;
            }
        }
    }

    // Write any remaining bits in the buffer.
    // This is crucial for the last byte of compressed data.
    if (bitCount > 0) {
        buffer <<= (8 - bitCount); // Pad with trailing zeros
        outputFile.put(buffer);
    }
}


void HuffmanCoding::compress(const std::string& inputFilePath, const std::string& outputFilePath) {
    std::ifstream inputFile(inputFilePath, std::ios::binary);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open input file: " << inputFilePath << std::endl;
        return;
    }

    std::ofstream outputFile(outputFilePath, std::ios::binary);
    if (!outputFile.is_open()) {
        std::cerr << "Error: Could not open output file: " << outputFilePath << std::endl;
        return;
    }

    std::cout << "Building frequency table..." << std::endl;
    buildFrequencyTable(inputFile);

    // Handle empty file as a special case.
    if (frequencyMap.empty()) {
        std::cout << "Input file is empty. Creating an empty compressed file." << std::endl;
        inputFile.close();
        outputFile.close();
        return;
    }

    std::cout << "Building Huffman tree..." << std::endl;
    buildHuffmanTree();

    std::cout << "Generating Huffman codes..." << std::endl;
    generateCodes(treeRoot, "");

    std::cout << "Writing header..." << std::endl;
    writeHeader(outputFile);

    std::cout << "Writing compressed data..." << std::endl;
    writeCompressedData(inputFile, outputFile);

    inputFile.close();
    outputFile.close();

    std::cout << "Compression successful!" << std::endl;
}


// --- Decompression Implementation ---

void HuffmanCoding::readHeader(std::ifstream& inputFile) {
    frequencyMap.clear();
    size_t mapSize;
    inputFile.read(reinterpret_cast<char*>(&mapSize), sizeof(mapSize));

    // Reconstruct the frequency map by reading from the header.
    for (size_t i = 0; i < mapSize; ++i) {
        char character;
        unsigned frequency;
        inputFile.read(&character, sizeof(character));
        inputFile.read(reinterpret_cast<char*>(&frequency), sizeof(frequency));
        frequencyMap[character] = frequency;
    }
}

void HuffmanCoding::decodeData(std::ifstream& inputFile, std::ofstream& outputFile) {
    // Calculate total number of characters in original file to know when to stop decoding.
    long long totalChars = 0;
    for (const auto& pair : frequencyMap) {
        totalChars += pair.second;
    }
    if (totalChars == 0) return;

    auto currentNode = treeRoot;
    char byte;
    long long decodedCount = 0;

    while (inputFile.get(byte) && decodedCount < totalChars) {
        // Process each bit of the byte read from the file.
        for (int i = 7; i >= 0; --i) {
            if (decodedCount >= totalChars) break;

            // Get the current bit (0 or 1).
            int bit = (byte >> i) & 1;

            // Traverse the tree based on the bit.
            if (bit == 0) {
                currentNode = currentNode->left;
            } else {
                currentNode = currentNode->right;
            }

            // If it's a leaf node, we've found a character.
            if (currentNode && !currentNode->left && !currentNode->right) {
                outputFile.put(currentNode->data);
                decodedCount++;
                currentNode = treeRoot; // Return to the root for the next character.
            }
        }
    }
}


void HuffmanCoding::decompress(const std::string& inputFilePath, const std::string& outputFilePath) {
    std::ifstream inputFile(inputFilePath, std::ios::binary);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open input file: " << inputFilePath << std::endl;
        return;
    }

    std::ofstream outputFile(outputFilePath, std::ios::binary);
    if (!outputFile.is_open()) {
        std::cerr << "Error: Could not open output file: " << outputFilePath << std::endl;
        return;
    }

    std::cout << "Reading header..." << std::endl;
    readHeader(inputFile);

    std::cout << "Rebuilding Huffman tree..." << std::endl;
    buildHuffmanTree();

    std::cout << "Decoding data..." << std::endl;
    decodeData(inputFile, outputFile);

    inputFile.close();
    outputFile.close();

    std::cout << "Decompression successful!" << std::endl;
}


// --- Main Program ---
void showUsage() {
    std::cout << "Usage: huffman <command> <input_file> <output_file>" << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  c, compress      Compress the input file." << std::endl;
    std::cout << "  d, decompress    Decompress the input file." << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        showUsage();
        return 1;
    }

    std::string command = argv[1];
    std::string inputFile = argv[2];
    std::string outputFile = argv[3];

    HuffmanCoding hf;

    if (command == "c" || command == "compress") {
        hf.compress(inputFile, outputFile);
    } else if (command == "d" || command == "decompress") {
        hf.decompress(inputFile, outputFile);
    } else {
        std::cerr << "Error: Invalid command '" << command << "'" << std::endl;
        showUsage();
        return 1;
    }

    return 0;
}