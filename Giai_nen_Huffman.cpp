#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <fstream>

using namespace std;

struct Node {
    char ch;
    int freq;
    Node *left, *right;
    Node(char c, int f, Node* l = nullptr, Node* r = nullptr) {
        ch = c; freq = f; left = l; right = r;
    }
};

struct Compare {
    bool operator()(Node* l, Node* r) { return l->freq > r->freq; }
};
Node* buildHuffmanTree(const vector<int>& freq_table) {
    priority_queue<Node*, vector<Node*>, Compare> pq;
    for (int i = 0; i < 256; ++i) {
        if (freq_table[i] > 0) pq.push(new Node((char)i, freq_table[i]));
    }
    if (pq.empty()) return nullptr;
    while (pq.size() > 1) {
        Node* l = pq.top(); pq.pop();
        Node* r = pq.top(); pq.pop();
        pq.push(new Node('\0', l->freq + r->freq, l, r));
    }
    return pq.top();
}

void decompressFile(const string& inputFile, const string& outputFile) {
    ifstream inFile(inputFile, ios::binary);
    if (!inFile) {
        cerr << "LOI: Khong mo duoc file nen: " << inputFile << endl;
        return;
    }
    vector<int> freq_table(256);
    inFile.read((char*)freq_table.data(), 256 * sizeof(int));
    if (!inFile) {
        cerr << "LOI: File qua ngan hoac thieu Header!" << endl; return;
    }
    Node* root = buildHuffmanTree(freq_table);
    if (!root) {
        cerr << "LOI: Header khong hop le (Cay Huffman rong)!" << endl; return;
    }
    long long total_bits;
    inFile.read((char*)&total_bits, sizeof(long long));
    cout << "Tong so bit can giai ma: " << total_bits << endl;

    // 4. GIẢI NÉN
    ofstream outFile(outputFile, ios::binary);
    Node* current = root;
    long long bits_processed = 0;
    char byte;
    long long char_count = 0;
    while (inFile.get(byte)) {
        for (int i = 7; i >= 0; --i) {
            if (bits_processed >= total_bits) break; // Dừng nếu đã đủ bit
            int bit = (byte >> i) & 1;
            if (bit == 0) current = current->left;
            else          current = current->right;
            if (!current->left && !current->right) {
                outFile.put(current->ch);
                current = root; // Reset về gốc để tìm ký tự tiếp theo
                char_count++;
            }
            bits_processed++;
        }
    }

    inFile.close();
    outFile.close();
    cout << "--> GIAI NEN THANH CONG! (" << char_count << " ky tu)" << endl;
    cout << "File duoc luu tai: " << outputFile << endl;
}

int main() {
    // SỬA ĐƯỜNG DẪN CỦA BẠN TẠI ĐÂY
    string compressedFile = "/Users/huongne/Documents/For_xcode/TTSS/Huffman/Huffman/ket_qua_nen.bin";
    string recoveredFile  = "/Users/huongne/Documents/For_xcode/TTSS/Huffman/Giai_nen_Huffman/Giai_nen_Huffman/Input_khoiphuc.txt";
    decompressFile(compressedFile, recoveredFile);
    return 0;
}
