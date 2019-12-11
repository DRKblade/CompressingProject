#pragma once
#include <bits/stdc++.h>

using namespace std;

// kiểu dữ liệu mã nhị phân có độ dài tùy ý
struct huffcode{
    // giá trị mã
    unsigned int value;
    //độ dài
    unsigned char length;

    huffcode():value(0), length(0){}

    // thêm bit giá trị 1 vào mã và trả về mã mới
    huffcode branch_right(){
        huffcode temp = *this;
        temp.value |= 1u<<(temp.length++);
        return temp;
    }
    // thêm bit giá trị 0 vào mã
    huffcode branch_left(){
        huffcode temp = *this;
        temp.value &= ~(1u<<(temp.length++));
        return temp;
    }

};
// một node trong cây huffman
struct huffman_node {
    // Ký tự node này ký hiệu
	unsigned char id{};
	// Số lần xuất hiện của ký tự đó (chỉ dùng khi tạo cây huffman)
	unsigned long freq{};
	// Mã huffman của node
	huffcode code;
	huffman_node* left{};
	huffman_node* right{};

};

typedef huffman_node* phuffman_node;

class huffman {
    class compare {
    public:
        bool operator() ( const phuffman_node& c1, const phuffman_node& c2 )const {
            return c1->freq > c2->freq;
        }
    };
protected:
    // Dãy các node huffman ban đầu
    huffman_node nodeArray[256];
    // biến để tạo câu huffman
    priority_queue<phuffman_node, vector<phuffman_node>, compare> pq;
    // câu huffman hoàn chỉnh
    phuffman_node root;
    // file vào và ra
	fstream fin, fout;
	// tên file vào và ra
	string finName, foutName;

	// khỏi tạo biến nodeArray
	void createNodeArray();
	// duyệt toàn bộ câu huffman, gán mã cho từng node
	void traverse(phuffman_node, huffcode);
    // xây dựng cây huffman từ các bộ đôi mã huffman và ký tự
    inline void buildTree(huffcode, unsigned char);
public:
	// khỏi tạo từ tên file vào và file ra
	huffman(string, string);
	// Khảo sát file vào để biết số lần xuất hiện của từng ký tự rồi thêm vào pq
	void createPq();
	// tạo cây huffman
	void createHuffmanTree();
	// gán mã cho các node trong cây huffman
	void calculateHufmanCodes();
	// sử dụng cây huffman để mã hóa file input thành output
	void codingSave();
	// giải mã file input thành file output
	void decodingSave();
};
bool file_check(string file1, string file2);
