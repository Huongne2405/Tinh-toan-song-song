#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <map>
#include <chrono>
#include <fstream>

using namespace std;

struct Node {
    char ch;
    int freq;
    Node *left, *right;

    Node(char c, int f, Node* l = nullptr, Node* r = nullptr) {
        ch = c;
        freq = f;
        left = l;
        right = r;
    }
};

struct Compare {
    bool operator()(Node* l, Node* r) {
        return l->freq > r->freq;
    }
};
string readFile(const string& filename) {
    ifstream file(filename, ios::binary | ios::ate);
    
    if (!file) {
        cerr << "LOI: Khong the mo file '" << filename << "'." << endl;
        exit(1);
    }
    streamsize size = file.tellg();
    file.seekg(0, ios::beg);
    string buffer(size, '\0');
    if (file.read(&buffer[0], size)) {
        return buffer;
    }
    return "";
}
// Thống kê tần suất
void calcFrequencies(const string& text, vector<int>& freq_table) {
    for (size_t i = 0; i < text.length(); ++i) {
        freq_table[(unsigned char)text[i]]++;
    }
}
// Xây dựng cây Huffman
Node* buildHuffmanTree(const vector<int>& freq_table) {
    priority_queue<Node*, vector<Node*>, Compare> pq;

    for (int i = 0; i < 256; ++i) {
        if (freq_table[i] > 0) {
            pq.push(new Node((char)i, freq_table[i]));
        }
    }
    if (pq.empty()) return nullptr;

    while (pq.size() > 1) {
        Node* left = pq.top(); pq.pop();
        Node* right = pq.top(); pq.pop();
        int sum = left->freq + right->freq;
        Node* parent = new Node('\0', sum, left, right);
        pq.push(parent);
    }
    return pq.top();
}
// Tạo bảng mã
void generateCodes(Node* root, string str, map<char, string>& huffmanCode) {
    if (!root) return;
    if (!root->left && !root->right) {
        huffmanCode[root->ch] = str;
    }
    generateCodes(root->left, str + "0", huffmanCode);
    generateCodes(root->right, str + "1", huffmanCode);
}
// Mã hoá dữ liệu
string encodeText(const string& text, const map<char, string>& huffmanCode) {
    string encodedStr = "";
    for (char c : text) {
        encodedStr += huffmanCode.at(c);
    }
    return encodedStr;
}
void deleteTree(Node* root) {
    if (!root) return;
    deleteTree(root->left);
    deleteTree(root->right);
    delete root;
}
int main() {
    string inputPath = "/Users/huongne/Documents/For_xcode/TTSS/Huffman/Huffman/Input.txt";
    string outputPath = "/Users/huongne/Documents/For_xcode/TTSS/Huffman/Huffman_tuan_tu/Huffman_tuan_tu/ket_qua_nen.bin";
    cout << "Dang doc file: " << inputPath << endl;
    string text = readFile(inputPath);
    if (text.empty()) {
        cout << "File rong hoac khong doc duoc du lieu!" << endl;
        return 0;
    }
    cout << "Kích thước dữ liệu đầu vào: " << text.length()*8 << " bits" << endl;
    auto start = chrono::high_resolution_clock::now();

    // thống kê tần suất
    vector<int> freq_table(256, 0);
    calcFrequencies(text, freq_table);
    // Xây cây Huffman
    Node* root = buildHuffmanTree(freq_table);
    //Tạo bảng mã
    map<char, string> huffmanCode;
    generateCodes(root, "", huffmanCode);
    //Mã hóa
    string encodedString = encodeText(text, huffmanCode);

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    cout << "\n Kết quả chương trình tuần tự" << endl;
    cout << "Thời gian chạy ( Chỉ tính thuật toán): " << duration.count() << " giây" << endl;
    cout << "\n Độ dài chuỗi sau khi nén (bits): " << encodedString.length() << endl;
    double original_bits = (double)text.length() * 8.0;
    double compressed_bits = (double)encodedString.length();
    cout << "Tỷ lệ nén: " << (compressed_bits / original_bits) * 100 << "% Kích thước gốc" << endl;
    deleteTree(root);
    return 0;
}
