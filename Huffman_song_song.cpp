#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <map>
#include <omp.h>    // OpenMP
#include <fstream>
#include <sstream>
#include <chrono>   // Đo thời gian

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

void deleteTree(Node* root) {
    if (!root) return;
    deleteTree(root->left);
    deleteTree(root->right);
    delete root;
}

string readFile(const string& filename) {
    ifstream file(filename, ios::binary);
    if (!file) {
        cerr << "LOI: Khong the mo file " << filename << endl;
        return "";
    }
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void calcFrequenciesParallel(const string& text, vector<int>& freq_table, int num_threads) {
    long n = text.length();
    #pragma omp parallel num_threads(num_threads)
    {
        vector<int> local_freq(256, 0);
        #pragma omp for
        for (long i = 0; i < n; ++i) {
            local_freq[(unsigned char)text[i]]++;
        }
        #pragma omp critical
        {
            for (int i = 0; i < 256; ++i) freq_table[i] += local_freq[i];
        }
    }
}

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

void generateCodes(Node* root, string str, map<char, string>& huffmanCode) {
    if (!root) return;
    if (!root->left && !root->right) huffmanCode[root->ch] = str;
    generateCodes(root->left, str + "0", huffmanCode);
    generateCodes(root->right, str + "1", huffmanCode);
}
string encodeTextParallel(const string& text, const map<char, string>& huffmanCode, int num_threads) {
    long n = text.length();
    vector<string> partial_results(num_threads);
    #pragma omp parallel num_threads(num_threads)
    {
        int tid = omp_get_thread_num();
        long chunk = n / num_threads;
        long start = tid * chunk;
        long end = (tid == num_threads - 1) ? n : start + chunk;

        string local_res = "";
        local_res.reserve((end - start) * 8);
        for (long i = start; i < end; ++i) {
            local_res += huffmanCode.at(text[i]);
        }
        partial_results[tid] = local_res;
    }
    
    string final_result = "";
    for (int i = 0; i < num_threads; ++i) final_result += partial_results[i];
    return final_result;
}

void saveCompressedFile(const string& filename, const string& bitString, const vector<int>& freq_table) {
    ofstream outFile(filename, ios::binary);
    if (!outFile) { cerr << "Loi tao file output!" << endl; return; }
    outFile.write((char*)freq_table.data(), 256 * sizeof(int));
    long long total = bitString.length();
    outFile.write((char*)&total, sizeof(long long));
    unsigned char buffer = 0;
    int count = 0;
    for (char c : bitString) {
        buffer = buffer << 1;
        if (c == '1') buffer |= 1;
        count++;
        if (count == 8) { outFile.put(buffer); buffer = 0; count = 0; }
    }
    if (count > 0) { buffer = buffer << (8 - count); outFile.put(buffer); }
    
    outFile.close();
    cout << "Đã lưu file nén tại: " << filename << endl;
}
int main() {
    string inputPath = "/Users/huongne/Documents/For_xcode/TTSS/Huffman/Huffman/Input.txt";
    string outputPath = "/Users/huongne/Documents/For_xcode/TTSS/Huffman/Huffman/ket_qua_nen.bin";
    int num_threads = 4;
    omp_set_num_threads(num_threads);
    cout << "Chương trình nén  Huffman song song " << endl;
    cout << "Đang đọc file: " << inputPath << endl;
    string text = readFile(inputPath);
    if (text.empty()) return 1;
    cout << "Kích thước gốc: " << text.length()*8 << "(bits)" << endl;
    cout << "Số luồng đang sử dụng: " << num_threads << endl;
    auto start = chrono::high_resolution_clock::now();

    vector<int> freq(256, 0);
    calcFrequenciesParallel(text, freq, num_threads);

    Node* root = buildHuffmanTree(freq);
    map<char, string> codes;
    generateCodes(root, "", codes);
    string encoded = encodeTextParallel(text, codes, num_threads);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;
    cout << "Thời gian thực hiện (Chỉ có thuật toán): " << elapsed.count() << " giay <<<" << endl;
    
    saveCompressedFile(outputPath, encoded, freq);
    long compressed_size = encoded.length();
    double original_bits = (double)text.length() * 8.0;
    cout << "Kích thước sau khi nén: " << compressed_size << "(bits)" << endl;
    cout << "Tỷ lệ nén : " << ((double)compressed_size / original_bits) * 100.0 << " %" << endl;

    deleteTree(root);
    return 0;
}
