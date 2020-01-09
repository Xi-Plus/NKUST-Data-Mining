#ifndef APRIORI_H
#define APRIORI_H

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class Apriori {
   private:
	struct Node {
		unsigned int level;
		std::unordered_map<int, Node *> child;
	};
	struct vector_hash {
		size_t operator()(const std::vector<unsigned int> &items) const {
			std::hash<unsigned int> hasher;
			size_t res = 0;
			for (auto &item : items) {
				res ^= hasher(item) + 0x9e3779b9 + (res << 4) + (res >> 5);
			}
			return res;
		}
	};

	// Arguments
	char inputpath[100], outputpath[100];
	unsigned int support;
	std::fstream fin, fout;
	// Data
	std::unordered_map<int, int> C1;
	std::unordered_map<Node *, int> Csup;
	std::vector<std::vector<unsigned int>> Ctemp, Cits;
	std::unordered_set<std::vector<unsigned int>, vector_hash> Lset;
	Node *root;
	int grouplen = 1;
	unsigned int Llen = 0;
	// Setting
	bool isShowDetailedTime = false;
	unsigned int CtempMin = 3;
	unsigned int CtempMax = 3;
	// Temporary variables
	int tempa, tempb, tempc;
	unsigned int tempn, cnt;
	bool inTrie;
	Node *tempNode;
	std::vector<unsigned int> tempv;

   public:
	Apriori(char *_inputpath, char *_outputpath, unsigned int _support);
	unsigned int run();
	void showDetailedTime(bool setting);

   private:
	inline unsigned int readint();

	void generateC1();
	void generateL1();
	void dfsOutputFile(Node *&now, std::vector<unsigned int> item);
	void outputFile();
	void generateC();
	void generateCsup();
	void dfsGenerateL(Node *&now, std::vector<unsigned int> item);
	void generateL();
	// Debug
	void dfsPrintTrie(Node *&now, std::vector<unsigned int> item);
	void printTrie();
};

#endif
