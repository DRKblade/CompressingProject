#include "huffman.h"

void huffman::createNodeArray()
{
	for (int i = 0; i < 256; i++)
	{
	    // mỗi node ứng với một ký tự
		nodeArray[i].id = i;
		nodeArray[i].freq = 0;
	}
}
void huffman::traverse(phuffman_node node, huffcode code)
{
	if (node->left == NULL && node->right == NULL)
		node->code = code;
	else
	{
		traverse(node->left, code.branch_left());
		traverse(node->right, code.branch_right());
	}
}

inline void huffman::buildTree(huffcode path, unsigned char aCode)
{
	phuffman_node current = root;
	for (unsigned int i = 0; i < path.length; i++)
	{
		if (path.value & 1u<<i)
		{
            if (current->right == nullptr)
                current->right = new huffman_node;
            current = current->right;
		} else {
            if (current->left == nullptr)
                current->left = new huffman_node;
            current = current->left;
		}
	}
	current->id = aCode;
	current->code = path;
}
huffman::huffman(string in, string out)
{
	finName = in;
	foutName = out;
	createNodeArray();
}
void huffman::createPq()
{
    unsigned char id;
	fin.open(finName, ios::in);
	if (!fin)
	{
		cerr << "Error!" << endl;
		return;
	}
	while (!fin.eof())
	{
        fin.get((char&)id);
		nodeArray[id].freq++;
	}
	fin.close();
	for (int i = 0; i < 256; i++)
	{
		if (nodeArray[i].freq)
			pq.push(&nodeArray[i]);
	}
}
void huffman::createHuffmanTree()
{
	priority_queue<phuffman_node, vector<phuffman_node>, compare> temp(pq);
	while (temp.size() > 1)
	{
		root = new huffman_node;
		root->freq = 0;
		root->left = temp.top();
		root->freq += temp.top()->freq;
		temp.pop();
		root->right = temp.top();
		root->freq += temp.top()->freq;
		temp.pop();
		temp.push(root);
	}
}
void huffman::calculateHufmanCodes()
{
	traverse(root, huffcode());
}
void write_tree(phuffman_node node, ostream& stream){
    if(node) {
        if(node->left == NULL && node->right == NULL){
            stream.write((char*)&node->id, 1);
            stream.write((char*)&node->code, sizeof(huffcode));
            cout<<(int)node->code.length<<endl;
        } else {
            write_tree(node->left, stream);
            write_tree(node->right, stream);
        }
    }
}
void huffman::codingSave()
{
	fin.open(finName, ios::in);
	fout.open(foutName, ios::out | ios::binary);
	if (!fin||!fout)
	{
		cerr << "Error!" << endl;
		return;
	}
    fin.seekg(0, ios_base::seekdir::_S_end);
    unsigned long file_size = fin.tellg();
    fin.seekg(0, ios_base::seekdir::_S_beg);
    fout.write((char*)&file_size, sizeof(file_size));

	unsigned long long buffer = 0;
	unsigned short length = 0;
	unsigned char id;

	unsigned char temp_output = pq.size();
    unsigned short temp_short = pq.size();
	fout.write((char*)&temp_short, 2);
	write_tree(root, fout);
	while (!fin.eof())
	{
        fin.get((char&)id);
//        cout<<fin.tellg()<<" "<<fout.tellp()<<" in "<<(int)id<<" "<<nodeArray[id].code.value<<" "<<(int)nodeArray[id].code.length<<endl;
		buffer += (unsigned long long)nodeArray[id].code.value << (unsigned long long)length;
		length += nodeArray[id].code.length;
		if(length>31)
		while (length>=8)
		{
		    temp_output = buffer % 256;
            fout.write((char*)&temp_output, 1);
            buffer >>= 8;
            length -=8;
		}
	}
    temp_output = buffer % 256;
    fout.write((char*)&temp_output, 1);

	fin.close();
	fout.close();
}

void huffman::decodingSave()
{
	fin.open(finName, ios::in | ios::binary);
	fout.open(foutName, ios::out);
	if (!fin || !fout)
	{
		cerr << "Error!" << endl;
		return;
	}
    unsigned long file_size;
    fin.read((char*)&file_size, sizeof(file_size));

    unsigned short size;
    fin.read((char*)(&size), 2);
    root = new huffman_node;
    for (int i = 0; i < size; i++)
    {
        unsigned char newCode;
        huffcode code;
        fin.read((char*)&newCode, 1);
        fin.read((char*)(&code), sizeof(huffcode));
        buildTree(code, newCode);
    }

    auto current_pos = root;
    unsigned char value = 0, length = 8;
    for(unsigned long i = 0; i<file_size; i++) {
        while(current_pos->left!= NULL || current_pos->right!=NULL){
            if(length == 8){
                fin.read((char*)&value, 1);
                length = 0;
            }
            if(value & (1u<<length++))
                current_pos = current_pos->right;
            else current_pos = current_pos->left;
            if(current_pos == nullptr){
                cout<<"internal error 1";
                throw;
            }
        }
        fout.write((char*)&current_pos->id, 1);
//        cout<<fout.tellp()<<" "<<fin.tellg()<<" out "<<(int)current_pos->id<<" "<<current_pos->code.value<<" "<<(int)current_pos->code.length<<endl;
        current_pos = root;
    }
	fin.close();
	fout.close();
}

bool file_check(string file1, string file2) {
    auto f1 = fopen(file1.c_str(), "rb");
    auto f2 = fopen(file2.c_str(), "rb");

    if(!f1 || !f2)
        throw;

    char temp1=0, temp2=0;
    while(!feof(f1) && !feof(f2)) {
        fread(&temp1, 1, 1, f1);
        fread(&temp2, 1, 1, f2);
        if(temp1!=temp2)
            break;
    }
    if(temp1!=temp2)
        return false;
    return feof(f2) && feof(f1);
}
