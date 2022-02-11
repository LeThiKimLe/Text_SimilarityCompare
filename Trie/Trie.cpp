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

const int ALPHABET_SIZE = 36;
ofstream outfile1;
ofstream outfile2;
fs::path nowpath = fs::current_path().parent_path();
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
typedef struct word
{
	int belong;
	string value;
	bool match = false;
	bool isstopwword = false;
	void Print()
	{
		cout << value << " ";
	}
} *Word;
word* NewWord(string name, int num)
{
	word* temp = new word;
	temp->value = name;
	temp->belong = num;
	return temp;
}
struct TrieNode
{
	struct TrieNode* children[ALPHABET_SIZE];
	bool isLeaf = false;
	vector<int> belong;
};

// Returns new trie node (initialized to NULLs)
TrieNode* createNode()
{
	struct TrieNode* pNode = new TrieNode;

	for (int i = 0; i < ALPHABET_SIZE; i++)
		pNode->children[i] = NULL;

	pNode->isLeaf = false;

	return pNode;
};

// function to insert a node in Trie
void insert(struct TrieNode* root, word key)
{
	int level;
	int length = key.value.length();
	struct TrieNode* pCrawl = root;
	int index;
	for (level = 0; level < length; level++)
	{
		if (key.value[level] >= 'a' && key.value[level] <= 'z')
			index = key.value[level] - 'a';
		else
			index = key.value[level] - 'w' + 'a';

		if (pCrawl->children[index] == NULL)
			pCrawl->children[index] = createNode();

		pCrawl = pCrawl->children[index];
	}
	if (pCrawl->isLeaf == false)
	{
		pCrawl->isLeaf = true;
	}
	pCrawl->belong.push_back(key.belong);

}

// function to check if current node is leaf node or not
bool isLeafNode(struct TrieNode* root)
{
	return root->isLeaf != false;
}

// function to display the content of Trie
void display(struct TrieNode* root, char str[], int level)
{
	// If node is leaf node, it indicates end
	// of string, so a null character is added
	// and string is displayed
	if (isLeafNode(root))
	{
		str[level] = '\0';
		cout << str << " ";
		for (auto i : root->belong)
			cout << i << " ";
		cout << endl;
	}

	int i;
	for (i = 0; i < ALPHABET_SIZE; i++)
	{
		// if NON NULL child is found
		// add parent key to str and
		// call the display function recursively
		// for child node
		if (root->children[i])
		{
			if (i >= 0 && i <= 25)
				str[level] = i + 'a';
			else
				str[level] = i + 'w' - 'a';
			display(root->children[i], str, level + 1);
		}
	}
}
TrieNode* search(struct TrieNode* root, string key)
{
	struct TrieNode* pCrawl = root;
	int index;
	for (int i = 0; i < key.length(); i++)
	{
		if (key[i] >= 'a' && key[i] <= 'z')
			index = key[i] - 'a';
		else
			index = key[i] - 'w' + 'a';
		if (!pCrawl->children[index])
			return nullptr;

		pCrawl = pCrawl->children[index];
	}
	if (pCrawl->isLeaf == false)
		return nullptr;
	else
		return pCrawl;
}
typedef struct sentence
{
	int index = 0;
	bool containlastkey = true;
	bool checked = false;
	bool end = false;
	vector<Word> key;
	int count = 0;
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
	void Free()
	{
		for (Word i : key)
		{
			delete(i);
		}
	}
}*Sentence;
struct text
{
	struct TrieNode* root = createNode();
	vector<Sentence> sen;
	int count = 0;
	bool issour;
	void AddSentence(sentence* s)
	{
		if (issour == true)
			for (Word i : s->key)
			{
				if (i->isstopwword == false)
					insert(root, *i);
			}
		sen.push_back(s);
		s->index = sen.size() - 1;
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
	TrieNode* searchword(string x)
	{
		return search(root, x);
	}
	void deleteTree(struct TrieNode* node)
	{
		if (isLeafNode(node))
		{
			return;
		}
		int i;
		for (i = 0; i < ALPHABET_SIZE; i++)
		{
			if (node->children[i])
			{
				deleteTree(node->children[i]);
				delete(node->children[i]);
			}
		}
	}
	void Free()
	{
		deleteTree(root);
		for (Sentence i : sen)
		{
			i->Free();
			delete(i);
		}
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
	return m;
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
		//cout << dataout << endl;
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
	return tree;
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

	delete(v0);
	delete(v1);

	return kq;
}


int Findthefirst(Sentence s, text* sour)
{
	int found;
	int index;
	TrieNode* temp;
	int* track = new int[sour->count];
	for (int i = 0; i < sour->count; i++)
		track[i] = 0;
	for (Word i : s->key)
	{
		if (i->isstopwword == false)
		{
			temp = search(sour->root, i->value);
			if (temp != nullptr)
			{
				for (int j : temp->belong)
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
void Getfile(string text1, string text2)
{

	text* des;
	text* sour;

	long m = Getsize(text1);
	long n = Getsize(text2);

	float x = 0;
	bool issour = true;
	string desfile, sourfile;

	desfile = (m <= n) ? text1 : text2;
	sourfile = (desfile != text1) ? text1 : text2;

	des = CreateTree(desfile, !issour);

	auto start = high_resolution_clock::now();
	sour = CreateTree(sourfile, issour);
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<milliseconds>(stop - start);
	x = (float)duration.count() / 1000;
	outfile1 << setw(20) << x;

	if (des != nullptr && sour != nullptr)
	{
		auto start = high_resolution_clock::now();
		Compare(des, sour);
		auto stop = high_resolution_clock::now();
		auto duration = duration_cast<milliseconds>(stop - start);
		x = (float)duration.count() / 1000;
		outfile2 << setw(20) << x;
		des->Free();
		sour->Free();
		delete(des);
		delete(sour);
	}
}
int main()
{
	outfile1.open(nowpath / "Simulate" / "docTrie.txt", ios::out | ios::trunc);
	outfile2.open(nowpath / "Simulate" / "sosanhTrie.txt", ios::out | ios::trunc);

	fs::path x = nowpath / "Testfile";
	string x1, x2;

	long m, n;
	for (int i = 0; i < 30; i++)
	{
		x1 = (x / ("text" + std::to_string(i + 1) + ".txt")).string();
		x2 = (x / ("text" + std::to_string(i + 2) + ".txt")).string();
		m = Getsize(x1);
		n = Getsize(x2);
		outfile1 << m / 1024 << setw(20) << n / 1024;
		outfile2 << m / 1024 << setw(20) << n / 1024;
		for (int j = 0; j < 10; j++)
			Getfile(x1, x2);
		outfile1 << endl;
		outfile2 << endl;
	}
}

