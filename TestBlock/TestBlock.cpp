#include<iostream>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<string>
#include<deque>
#include<fstream>
#include<vector>
#include <sstream>
#include <iterator>
#include <numeric>
#include <algorithm>
#include<windows.h>
#include <cstdio>
#include <chrono>
#include <unordered_map>
#include <iomanip>
#include <filesystem>

using namespace std;
using namespace std::chrono;
namespace fs = std::filesystem;

ofstream outfile1;
ofstream outfile2;
float result;
fs::path nowpath = fs::current_path().parent_path();

typedef struct word
{
	int belong;
	string value;
	bool match = false;
	bool isstopwword = false;
	void Init(string name, int num)
	{
		value = name;
		belong = num;
	}
	void Print()
	{

		if (match == true)
		{
			HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
			SetConsoleTextAttribute(hConsole, 10);
			cout << value << " ";
		}
		else
		{
			HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
			SetConsoleTextAttribute(hConsole, 12);
			cout << value << " ";
		}
	}
	void Reset()
	{
		match = false;
	}
} *Word;
word* NewWord(string name, int num)
{
	Word temp = new word;
	temp->Init(name, num);
	return temp;
}
typedef struct sentence
{
	int index = 0;
	vector<Word> key;
	int count = 0;
	bool end = false;
	bool checked = false;
	void AddWord(Word x)
	{
		key.push_back(x);
		count++;
	}
	void Print()
	{
		cout << "   ";
		for (Word i : key)
		{
			i->Print();
		}
	}
	void Reset()
	{
		for (Word i : key)
		{
			i->Reset();
			i->match = false;
		}
	}
	void Free()
	{
		for (Word i : key)
		{
			delete(i);
		}
	}
	void Reset2()
	{
		checked = false;
		for (auto i : key)
			i->Reset();
	}

} *Sentence;

struct Bword
{
	vector<int> belong;
	string value;

	void Init(string name, int num)
	{
		value = name;
		belong.push_back(num);
	}
};
Bword* NewBWord(word in)
{
	Bword* temp = new Bword;
	temp->Init(in.value, in.belong);
	return temp;
}
class BTreeNode
{
	int t;	 // Minimum degree (defines the range for number of keys)
	BTreeNode** C; // An array of child pointers
	int n;	 // Current number of keys
	bool leaf; // Is true when node is leaf. Otherwise false
public:
	Bword* keys;
	BTreeNode(int _t, bool _leaf); // Constructor

	// A utility function to insert a new key in the subtree rooted with
	// this node. The assumption is, the node must be non-full when this
	// function is called
	void insertNonFull(word k);
	// A utility function to split the child y of this node. i is index of y in
	// child array C[]. The Child y must be full when this function is called
	void splitChild(int i, BTreeNode* y);
	void traverse();
	// A function to search a key in the subtree rooted with this node.
	BTreeNode* search(string k, int& index);
	friend class BTree;
};
class BTree
{
	BTreeNode* root; // Pointer to root node
	int t; // Minimum degree
public:
	// Constructor (Initializes tree as empty)
	BTree(int _t)
	{
		root = NULL; t = _t;
	}
	// function to traverse the tree
	void traverse()
	{
		if (root != NULL) root->traverse();
	}
	// function to search a key in this tree
	BTreeNode* search(string k, int& index)
	{
		return (root == NULL) ? NULL : root->search(k, index);
	}
	// The main function that inserts a new key in this B-Tree
	void insert(word k);

};
// Constructor for BTreeNode class
BTreeNode::BTreeNode(int t1, bool leaf1)
{
	// Copy the given minimum degree and leaf property
	t = t1;
	leaf = leaf1;

	// Allocate memory for maximum number of possible keys
	// and child pointers
	keys = new Bword[2 * t - 1];
	C = new BTreeNode * [2 * t];
	// Initialize the number of keys as 0
	n = 0;
}

// Function to traverse all nodes in a subtree rooted with this node
void BTreeNode::traverse()
{
	// There are n keys and n+1 children, traverse through n keys
	// and first n children
	int i;
	for (i = 0; i < n; i++)
	{
		// If this is not leaf, then before printing key[i],
		// traverse the subtree rooted with child C[i].
		if (leaf == false)
			C[i]->traverse();

		cout << endl << keys[i].value;
		for (int j : keys[i].belong)
			cout << " " << j;
	}

	// Print the subtree rooted with last child
	if (leaf == false)
		C[i]->traverse();
}

// Function to search key k in subtree rooted with this node
BTreeNode* BTreeNode::search(string k, int& index)
{
	// Find the first key greater than or equal to k
	int i = 0;
	while (i < n)
	{
		if (k.compare(keys[i].value) > 0)
			i++;
		else
			break;
	}
	if (i < n)
		// If the found key is equal to k, return this node
		if (k.compare(keys[i].value) == 0)
		{
			index = i;
			return this;
		}
	// If key is not found here and this is a leaf node
	if (leaf == true)
		return NULL;
	// Go to the appropriate child
	return C[i]->search(k, index);
}

// The main function that inserts a new key in this B-Tree
void BTree::insert(word k)
{
	// If tree is empty

	if (root == NULL)
	{
		// Allocate memory for root
		root = new BTreeNode(t, true);
		root->keys[0] = *NewBWord(k); // Insert key
		root->n = 1; // Update number of keys in root
	}
	else // If tree is not empty
	{
		int index;
		BTreeNode* temp = search(k.value, index);
		if (temp != NULL)
		{
			temp->keys[index].belong.push_back(k.belong);
			return;
		}
		// If root is full, then tree grows in height
		if (root->n == 2 * t - 1)
		{
			// Allocate memory for new root
			BTreeNode* s = new BTreeNode(t, false);

			// Make old root as child of new root
			s->C[0] = root;

			// Split the old root and move 1 key to the new root
			s->splitChild(0, root);

			// New root has two children now. Decide which of the
			// two children is going to have new key
			int i = 0;
			if (s->keys[0].value.compare(k.value) < 0)
				i++;

			s->C[i]->insertNonFull(k);

			// Change root
			root = s;
		}
		else // If root is not full, call insertNonFull for root
			root->insertNonFull(k);
	}
}

// A utility function to insert a new key in this node
// The assumption is, the node must be non-full when this
// function is called
void BTreeNode::insertNonFull(word k)
{
	// Initialize index as index of rightmost element
	int i = n - 1;

	// If this is a leaf node
	if (leaf == true)
	{
		// The following loop does two things
		// a) Finds the location of new key to be inserted
		// b) Moves all greater keys to one place ahead
		while (i >= 0 && keys[i].value.compare(k.value) > 0)
		{
			keys[i + 1] = keys[i];
			i--;
		}

		// Insert the new key at found location
		keys[i + 1] = *NewBWord(k);
		n = n + 1;
	}
	else // If this node is not leaf
	{
		// Find the child which is going to have the new key
		while (i >= 0 && keys[i].value.compare(k.value) > 0)
			i--;

		// See if the found child is full
		if (C[i + 1]->n == 2 * t - 1)
		{
			// If the child is full, then split it
			splitChild(i + 1, C[i + 1]);

			// After split, the middle key of C[i] goes up and
			// C[i] is splitted into two. See which of the two
			// is going to have the new key
			if (keys[i + 1].value.compare(k.value) < 0)
				i++;

		}
		C[i + 1]->insertNonFull(k);
	}
}

// A utility function to split the child y of this node
// Note that y must be full when this function is called
void BTreeNode::splitChild(int i, BTreeNode* y)
{
	// Create a new node which is going to store (t-1) keys
	// of y
	BTreeNode* z = new BTreeNode(y->t, y->leaf);
	z->n = t - 1;
	// Copy the last (t-1) keys of y to z
	for (int j = 0; j < t - 1; j++)
		z->keys[j] = y->keys[j + t];
	// Copy the last t children of y to z
	if (y->leaf == false)
	{
		for (int j = 0; j < t; j++)
			z->C[j] = y->C[j + t];
	}
	// Reduce the number of keys in y
	y->n = t - 1;
	// Since this node is going to have a new child,
	// create space of new child
	for (int j = n; j >= i + 1; j--)
		C[j + 1] = C[j];
	// Link the new child to this node
	C[i + 1] = z;
	// A key of y will move to this node. Find the location of
	// new key and move all greater keys one space ahead
	for (int j = n - 1; j >= i; j--)
		keys[j + 1] = keys[j];
	// Copy the middle key of y to this node
	keys[i] = y->keys[t - 1];
	// Increment count of keys in this node
	n = n + 1;
}
string* loadwords(const string& filename)
{
	string* arr = new string[76];
	ifstream file(filename);
	string data;
	int i = 0;
	while (getline(file, data))
	{
		arr[i++] = data;
	}
	file.close();
	return arr;
}
bool lookup(string* words, const string& word)
{
	return binary_search(words, words + 76, word);
}
string* dictionary = loadwords("stopword.txt");
struct text
{
	BTree* tree = new BTree(3);
	vector<Sentence> sen;
	int count = 0;
	int meaning = 0;
	bool issour;
	int num_couple = 0;
	void AddSentence(Sentence& s)
	{
		int index;
		s->index = count;
		sen.push_back(s);
		if (issour == true)
			for (Word i : s->key)
			{
				if (i->isstopwword == false)
				{
					tree->insert(*i);
					meaning++;
				}
			}
		count++;
	}
	void Print()
	{
		for (Sentence i : sen)
		{
			cout << endl << endl;
			i->Print();
		}
	}
	void Free()
	{
		for (Sentence i : sen)
		{
			i->Free();
			delete(i);
		}
	}
	void Reset()
	{
		for (auto i : sen)
			i->Reset2();
	}
};

bool isletter(char c)
{
	return ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9'));
}
string pretreatment(string t)
{
	string temp;
	int n = 0;
	int len = t.length();
	for (int i = 0; i < len; i++)
	{
		if (t[i] >= 'A' && t[i] <= 'Z')
			t[i] = t[i] + 32;
		if (i == len - 1) {
			if (t[i] == '.')
				temp.push_back(t[i]);
		}
		if (isletter(t[i]) == true)
		{
			temp.push_back(t[i]);
		}
	}
	return temp;
}
int Getsize(const string name)
{
	long l, m;
	ifstream file(name, ios::in | ios::binary);
	l = file.tellg();
	file.seekg(0, ios::end);
	m = file.tellg();
	file.close();
	//cout << "Lay đc size";
	return m;
}
int LikelyIndex(int array[], text* sour)
{

	int imax = 0;
	int n = sour->count;
	for (int i = 0; i < n; i++)
	{
		if (array[i] > array[imax] || (array[i] == array[imax] && sour->sen[i]->count < sour->sen[imax]->count))
			imax = i;
	}

	if (array[imax] == 0)
		return -1;
	return imax;
}
struct Dif
{
	int count = 0;
	float value = -1;
};

typedef struct Blocky
{
	int key;
	long index;
	int first = -1;
	int last = first;
	int count = 0;
	bool match = false;
	void Add(int Y)
	{
		if (count == 0)
		{
			first = Y;
			last = first;
		}
		else
			last = Y;
		count++;
		return;
	}
} *BlockY;

typedef struct Blockx
{
	bool match = false;
	long index;
	int first = -1;
	int last = first;
	float value = 0;
	int count = 0;
	float similar = 0;
	int lastnum;
	BlockY y;
	void Add(int X, float Value)
	{
		if (count == 0)
		{
			first = X;
			last = first;
			value = Value;
		}
		else
		{
			last = X;
			value += Value;

		}
		lastnum = value;
		similar += 1 - Value;
		count++;
	}
	float Value(float num)
	{
		value = value - lastnum + num;
		similar = similar - (1 - lastnum) + (1 - num);
	}
} *BlockX;

struct Hash
{
	int count = 0;
	int width;
	int depth;
	int in = 0;

	unordered_map<long, Dif> map;

	vector<BlockX> t;
	vector<BlockY> v;

	BlockX nowX;
	BlockY nowY;

	bool iswait = false;
	int lastx;
	int lasty;

	int isempty = 1;
	float sum = 0;
	int lastkey = -1;

	vector<int> match;

	void Getnode(int x, int y, float value)
	{
		in++;
		if (x > isempty)
		{
			if (iswait == true) {
				v.push_back(nowY);
				t.push_back(nowX);
			}
			BlockX q = new Blockx;
			q->Add(isempty, 1);
			q->Add(x - 1, 1);
			q->count = q->last - q->first + 1;
			t.push_back(q);
			iswait = false;
		}
		if (x == lastx && y == lasty)
		{
			int t1 = t.size();
			if (value > t[t1 - 1]->value)
				return;
			else
			{
				t.pop_back();
				v.pop_back();
			}
		}
		if (value > 0.3)
		{
			if (iswait == true) {
				v.push_back(nowY);
				t.push_back(nowX);
			}

			BlockY n = new Blocky;
			n->Add(y);
			n->match = true;
			v.push_back(n);

			BlockX m = new Blockx;
			m->Add(x, value);
			m->y = n;
			m->match = true;
			t.push_back(m);
			lastx = x;
			lasty = y;
			iswait = false;
		}
		else
		{
			if (iswait == false)
			{
				BlockY n = new Blocky;
				n->Add(y);
				n->match = true;
				nowY = n;

				BlockX m = new Blockx;
				m->Add(x, value);
				m->y = n;
				m->match = true;
				nowX = m;

				lastx = x;
				lasty = y;
				iswait = true;
			}
			else
			{
				if (x == lastx + 1 && y == lasty + 1)
				{
					nowX->Add(x, value);
					nowY->Add(y);
					lastx = x;
					lasty = y;
				}
				else
				{
					v.push_back(nowY);
					t.push_back(nowX);

					BlockY n = new Blocky;
					n->Add(y);
					n->match = true;
					nowY = n;

					BlockX m = new Blockx;
					m->Add(x, value);
					m->y = n;
					m->match = true;
					nowX = m;

					lastx = x;
					lasty = y;
					iswait = true;
				}
			}
		}
		isempty = x + 1;
	}
	bool Ismatch(BlockX i, BlockY j)
	{
		return (i->y == j);
	}
	void Dataprocess()
	{

		if (in == 0)
			return;
		if (iswait == true)
		{
			v.push_back(nowY);
			t.push_back(nowX);
		}
		if (isempty <= depth)
		{
			BlockX q = new Blockx;
			q->Add(isempty, 1);
			q->Add(depth, 1);
			q->count = q->last - q->first + 1;
			t.push_back(q);
		}
		sort(v.begin(), v.end(), [](const BlockY lhs, const BlockY rhs) {
			return lhs->first < rhs->first;
			});

		isempty = 1;
		int tam = v.size();
		for (int j = 0; j < tam; j++)
		{
			if (v[j]->first > isempty)
			{
				BlockY q = new Blocky;
				q->Add(isempty);
				q->Add(v[j]->first - 1);
				q->count = q->last - q->first + 1;
				v.push_back(q);
			}
			isempty = v[j]->last + 1;
		}
		if (isempty <= width)
		{
			BlockY q = new Blocky;
			q->Add(isempty);
			q->Add(width);
			q->count = q->last - q->first + 1;
			v.push_back(q);
		}

		sort(v.begin(), v.end(), [](const BlockY lhs, const BlockY rhs) {
			return lhs->first < rhs->first;
			});

		int index = 0;

		depth = t.size();
		for (int i = 0; i < depth; i++)
		{
			t[i]->index = i;
		}
		for (int i = 0; i < v.size(); i++)
		{
			if (i != 0)
				if (v[i]->first != v[i - 1]->first)
				{
					index++;
				}
				else
				{
					for (auto j : t)
						if (Ismatch(j, v[i]))
						{
							j->y = v[i - 1];
							v.erase(v.begin() + i);
							break;
						}
					i--;
				}
			v[i]->index = index;
		}

		width = index + 1;
		CreateHashmap();
	}
	void CreateHashmap()
	{
		int key;
		Dif z;
		int tam = t.size();
		for (int i = 0; i < tam; i++)
		{
			if (t[i]->match == true)
			{
				key = t[i]->index * width + t[i]->y->index;
				Dif z;
				z.value = t[i]->value;
				sum += t[i]->similar;
				map[key] = z;
				count++;
			}
		}
	}
	float Get(int x, int y)
	{
		long key = x * width + y;
		if (map[key].value == -1)
		{
			return v[y]->count;
		}
		else
			return map[key].value;
	}
	float Getx(int x)
	{
		return t[x]->count;
	}
	float Gety(int y)
	{
		return v[y]->count;
	}
	float avrg()
	{
		return sum;
	}
	void Print()
	{
		for (auto i : map)
			cout << "[" << i.first << "]" << ":" << i.second.value << endl;
	}
	void Free()
	{
		t.clear();
		v.clear();
		t.shrink_to_fit();
		v.shrink_to_fit();
		map.clear();
	}
};

Hash* CreateHash(int width, int depth)
{
	Hash* hash = new Hash;
	hash->width = width;
	hash->depth = depth;
	return hash;
}

float FindSimilarity(Sentence s1, Sentence s2)
{
	sentence s3 = *s1;
	sentence s4 = *s2;

	int len1 = s3.count;
	int len2 = s4.count;
	int maxlen = (len1 > len2) ? len1 : len2;

	int min1, cost, count = 0;
	int* v0 = new int[len2 + 1];
	int* v1 = new int[len2 + 1];

	for (int i = 0; i <= len2; i++)
		v0[i] = i;

	for (int i = 0; i <= len1 - 1; i++)
	{
		v1[0] = i + 1;
		for (int j = 0; j <= len2 - 1; j++)
		{

			if (s3.key[i]->value.compare(s4.key[j]->value) == 0)
			{
				if (s1->key[i]->match == false)
					count++;
				s1->key[i]->match = true;
				s2->key[j]->match = true;
				cost = 0;
			}
			else
				cost = 1;
			min1 = ((v0[j + 1] + 1) < (v1[j] + 1) ? v0[j + 1] + 1 : v1[j] + 1);
			v1[j + 1] = min1 < (v0[j] + cost) ? min1 : (v0[j] + cost);
		}
		for (int i = 0; i <= len2; i++)
			v0[i] = v1[i];
	}
	float kq1 = 1 - v0[len2] / (float)maxlen;
	float kq2 = count / (float)maxlen;
	float kq = (kq1 + kq2) / 2;

	if (kq < 0.3)
	{
		s1->Reset();
		s2->Reset();
	}
	delete(v0);
	delete(v1);

	return kq;
}


int Findthefirst(Sentence s, text* sour)
{
	int found;
	int index;
	BTreeNode* temp;
	int* track = new int[sour->count];
	for (int i = 0; i < sour->count; i++)
		track[i] = 0;
	for (Word i : s->key)
	{
		if (i->isstopwword == false)
		{
			temp = sour->tree->search(i->value, index);
			if (temp != NULL)
			{
				for (int j : temp->keys[index].belong)
					track[j]++;
			}
		}
	}
	found = LikelyIndex(track, sour);
	delete(track);
	return found;
}

void BigLevenstein(Hash* table, int maxlength)
{
	if (table->count == 0)
	{
		//cout << "Do giong nhau la: " << 0;
		result = 0;
		return;
	}
	else
	{
		int N = table->width;
		int M = table->depth;
		float cost, cost1, cost2, min1;
		int temp = 0;

		float* v0 = new float[N + 1];
		float* v1 = new float[N + 1];

		for (int i = 0; i <= N; i++)
		{
			if (i == 0)
			{
				v0[i] = 0;
				continue;
			}
			temp += table->Gety(i - 1);
			v0[i] = temp;
		}
		temp = 0;
		for (int i = 0; i <= M - 1; i++)
		{
			cost1 = table->Getx(i);
			temp += cost1;
			v1[0] = temp;
			for (int j = 0; j <= N - 1; j++)
			{
				cost = table->Get(i, j);
				cost2 = table->Gety(j);
				min1 = ((v0[j + 1] + cost1) < (v1[j] + cost2) ? v0[j + 1] + cost1 : v1[j] + cost2);
				v1[j + 1] = min1 < (v0[j] + cost) ? min1 : (v0[j] + cost);
			}
			for (int i = 0; i <= N; i++)
				v0[i] = v1[i];
		}
		float kq1 = 1 - v0[N] / (float)maxlength;
		if (kq1 < 0)
			kq1 = 0;
		float kq2 = table->avrg() / (float)maxlength;
		float kq = (kq1 + kq2) / 2;
		table->Free();
		result = kq;

	}
}

void Compare(text* des, text* sour)
{

	float tempvalue = 0;
	int max = (des->count > sour->count) ? des->count : sour->count;
	int count = 0;
	int temp = 0, temp1 = -1;
	int flag = 0;
	int i = 0;
	int old = -1;
	Hash* table = CreateHash(sour->count, des->count);
	while (count < des->count)
	{

		if (des->sen[count]->checked == false)
		{
			temp = Findthefirst(des->sen[count], sour);
			des->sen[count]->checked = true;
		}
		else
		{
			temp1 = -1;
			count++;
			continue;
		}
		if (temp != -1)
		{
			if (temp != temp1)
			{
				tempvalue = FindSimilarity(des->sen[count], sour->sen[temp]);
				flag = 0;
				temp1 = -1;
			}
			else
			{
				temp1 = -1;
				count++;
				continue;
			}

			while (tempvalue >= 0.7)
			{
				table->Getnode(count + 1, temp + 1, 1 - tempvalue);

				if (count < des->count - 1 && temp < sour->count - 1)
				{
					count++;
					temp++;
				}
				else
				{
					count++;
					temp++;
					flag = 1;
					break;
				}
				tempvalue = FindSimilarity(des->sen[count], sour->sen[temp]);
				flag = 0;
			}
			if (flag == 0 && tempvalue >= 0.3)
			{
				table->Getnode(count + 1, temp + 1, 1 - tempvalue);
				old = tempvalue;
				temp1 = temp;
			}
			else if (flag == 0 && tempvalue < 0.3)
				if (des->sen[count]->checked == true)
					count++;
		}
		else
		{
			count++;
		}
	}

	table->Dataprocess();
	BigLevenstein(table, max);

}
void ReadLine(text*& tree, string datain, sentence*& last)
{
	sentence* t;
	t = last;
	word* c;
	int len;
	string dataout;
	datain = datain + ' ';
	std::stringstream ss(datain);
	while (getline(ss, dataout, ' '))
	{
		dataout = pretreatment(dataout);

		if (dataout.length() == 0)
		{
			continue;
		}
		len = dataout.length();

		if (dataout[len - 1] == '.')
		{
			dataout.pop_back();
			c = NewWord(dataout, tree->count);
			if (lookup(dictionary, dataout) == true)
				c->isstopwword = true;
			t->AddWord(c);
			t->end = true;
			tree->AddSentence(t);
			t = new sentence;
		}
		else
		{
			c = NewWord(dataout, tree->count);
			if (lookup(dictionary, dataout) == true)
				c->isstopwword = true;
			t->AddWord(c);
		}
	}
	last = t;
}
text* CreateTree(const string name, bool issource)
{
	text* tree = new text;
	tree->issour = issource;
	sentence* lastsen = new sentence;
	word* c;
	int len;
	string datain, dataout;
	ifstream file(name);
	if (!file.is_open())
	{
		cout << "Ko mo dc file";
		return nullptr;
	}
	while (getline(file, datain))
	{
		ReadLine(tree, datain, lastsen);
	}

	if (lastsen->count != 0)
	{
		tree->AddSentence(lastsen);
	}
	file.close();
	return tree;
}

void Getfile(string text1, string text2)
{

	text* des;
	text* sour;

	long m = Getsize(text1);
	long n = Getsize(text2);

	float x = 0;
	bool issour = true;
	if (m <= n)
	{
		des = CreateTree(text1, !issour);
		sour = CreateTree(text2, issour);
	}
	else
	{
		des = CreateTree(text2, !issour);
		sour = CreateTree(text1, issour);
	}

	if (des != nullptr && sour != nullptr)
	{
		for (int z = 0; z < 10; z++) {
			auto start = high_resolution_clock::now();
			Compare(des, sour);
			auto stop = high_resolution_clock::now();
			auto duration = duration_cast<milliseconds>(stop - start);
			x = (float)duration.count() / 1000;
			des->Reset();
			sour->Reset();
			outfile1 << setw(20) << x;
		}
		outfile2 << setw(20) << result;
		des->Free();
		sour->Free();
		delete(des);
		delete(sour);
	}
}

int main()
{
	outfile1.open(nowpath/"Simulate"/"timecompareBlock.txt", ios::out | ios::trunc);
	outfile2.open(nowpath/"Simulate"/"kqBlock.txt", ios::out | ios::trunc);

	fs::path x = nowpath/"Testfile";
	string x1, x2;

	long m, n;
	for (int i = 0; i < 15; i++)
	{
		x1 = (x / ("text" + std::to_string(i + 1) + ".txt")).string();
		x2 = (x / ("text" + std::to_string(i + 2) + ".txt")).string();
		m = Getsize(x1);
		n = Getsize(x2);
		outfile1 << m / 1024 << setw(20) << n / 1024;
		outfile2 << m / 1024 << setw(20) << n / 1024;
		Getfile(x1, x2);
		outfile1 << endl;
		outfile2 << endl;
	}
}
