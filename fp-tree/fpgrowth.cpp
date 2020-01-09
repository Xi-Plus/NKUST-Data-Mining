#include <algorithm>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

#define ROOT_VALUE 2147483647
// #define OUTPUT_ASCII
// #define TEST_SMALL

class FPGrowth {
   private:
	struct TreeNode {
		unsigned int item;
		unsigned int count;
		unordered_map<int, TreeNode *> child;
		TreeNode *next = nullptr;
		TreeNode *parent = nullptr;
		TreeNode(unsigned int _item) {
			item = _item;
			count = 0;
		}
		TreeNode(unsigned int _item, unsigned int _count) {
			item = _item;
			count = _count;
		}
		~TreeNode() {
			child.clear();
		}
	};
	struct HeaderTableNode {
		TreeNode *start = nullptr;
		TreeNode *end = nullptr;
	};
	struct Tree {
		vector<unsigned int> prefix;
		TreeNode *tree;
		unordered_map<unsigned int, HeaderTableNode *> header_table_pointer;
		Tree(vector<unsigned int> _prefix) {
			prefix = _prefix;
			tree = new TreeNode(ROOT_VALUE);
		}
		~Tree() {
			header_table_pointer.clear();
			free(tree);
		}
	};

	// Arguments
	char inputpath[100], outputpath[100];
	unsigned int support;
	fstream fin, fout;
	// Data
	unordered_map<unsigned int, unsigned int> C1;
	vector<pair<unsigned int, unsigned int>> L1;
	Tree *forest;
	unsigned int Llensum = 0;

	int tempa;
	unsigned int tempn, cnt;

   public:
	FPGrowth(char *_inputpath, char *_outputpath, unsigned int _support) {
		strcpy(inputpath, _inputpath);
		strcpy(outputpath, _outputpath);
		support = _support;
	}
	unsigned int run() {
		std::cout.setf(std::ios::unitbuf);

		fout.open(outputpath, fstream::out);
		if (!fout.is_open()) {
			cerr << "Fail to open output file";
			exit(1);
		}

		forest = new Tree(vector<unsigned int>(0, 0));

		Llensum = 0;

		generateC1();
		generateL1();
		cout << "L1: " << L1.size() << endl;

		buildTree();

		for (auto it = forest->header_table_pointer.begin(); it != forest->header_table_pointer.end(); it++) {
			buildSubTree(forest, it->first);
		}

		fout.close();

		return Llensum;
	}

   private:
	void generateC1() {
		fin.open(inputpath, fstream::in | fstream::binary);
		if (!fin.is_open()) {
			cerr << "Fail to open input file";
			exit(1);
		}

		std::string temps;
		while (getline(fin, temps)) {
			std::stringstream tempss(temps);
			while (tempss >> tempn) {
				C1[tempn]++;
			}
		}
		fin.close();
	}

	static bool L1Cmp(const pair<int, int> &a, const pair<int, int> &b) {
		return a.second > b.second;
	}

	void generateL1() {
		for (auto &v : C1) {
			if (v.second >= support) {
				L1.push_back(make_pair(v.first, v.second));
			}
		}
		sort(L1.begin(), L1.end(), L1Cmp);
#ifdef TEST_SMALL
		L1.clear();
		L1.push_back(make_pair(5, 4));
		L1.push_back(make_pair(2, 4));
		L1.push_back(make_pair(0, 3));
		L1.push_back(make_pair(1, 3));
		L1.push_back(make_pair(12, 3));
		L1.push_back(make_pair(15, 3));
#endif
		for (auto &v : L1) {
			HeaderTableNode *htn = new HeaderTableNode();
			forest->header_table_pointer[v.first] = htn;
		}
	}

	void buildTree() {
		fin.open(inputpath, fstream::in | fstream::binary);
		if (!fin.is_open()) {
			cerr << "Fail to open input file";
			exit(1);
		}

		std::string temps;
		while (getline(fin, temps)) {
			std::stringstream tempss(temps);
			unordered_map<unsigned int, bool> items;
			while (tempss >> tempn) {
				if (forest->header_table_pointer.find(tempn) != forest->header_table_pointer.end()) {
					items[tempn] = true;
				}
			}
			TreeNode *now = forest->tree;
			for (auto &item : L1) {
				if (items.find(item.first) != items.end()) {
					now = addItemToTreeNode(forest, now, item.first);
				}
			}
		}
		fin.close();
	}

	TreeNode *addItemToTreeNode(Tree *tree, TreeNode *treeNode, unsigned int item, unsigned int count = 1) {
		if (treeNode->child.find(item) == treeNode->child.end()) {
			treeNode->child[item] = new TreeNode(item, count);
			treeNode->child[item]->parent = treeNode;
			if (tree->header_table_pointer.find(item) == tree->header_table_pointer.end()) {
				tree->header_table_pointer[item] = new HeaderTableNode();
			}
			if (tree->header_table_pointer[item]->end == nullptr) {
				tree->header_table_pointer[item]->start = treeNode->child[item];
				tree->header_table_pointer[item]->end = treeNode->child[item];
			} else {
				tree->header_table_pointer[item]->end->next = treeNode->child[item];
				tree->header_table_pointer[item]->end = treeNode->child[item];
			}
		} else {
			treeNode->child[item]->count += count;
		}
		return treeNode->child[item];
	}

	void buildSubTree(Tree *fromTree, unsigned int leafItem) {
		TreeNode *leaf = fromTree->header_table_pointer[leafItem]->start;

		unordered_map<unsigned int, unsigned int> count;
		while (leaf != nullptr) {
			count[leaf->item] += leaf->count;

			TreeNode *now = leaf->parent;
			while (now->parent != nullptr) {
				count[now->item] += leaf->count;
				now = now->parent;
			}

			leaf = leaf->next;
		}

		// Output file
		for (auto &v : fromTree->prefix) {
			fout << _format_char(v) << ",";
		}
		fout << _format_char(leafItem);
		fout << ":" << count[leafItem] << "\n";
		Llensum++;

		// Create subTree
		vector<unsigned int> prefix(fromTree->prefix);
		prefix.push_back(leafItem);
		Tree *subTree = new Tree(prefix);

		leaf = fromTree->header_table_pointer[leafItem]->start;
		while (leaf != nullptr) {
			TreeNode *now = leaf->parent;
			vector<pair<unsigned int, unsigned int>> pathItems;
			while (now->parent != nullptr) {
				pathItems.push_back(make_pair(now->item, leaf->count));
				now = now->parent;
			}

			reverse(pathItems.begin(), pathItems.end());
			now = subTree->tree;
			for (auto &item : pathItems) {
				if (count[item.first] < support) {
					continue;
				}
				now = addItemToTreeNode(subTree, now, item.first, item.second);
			}

			leaf = leaf->next;
		}

		if (subTree->header_table_pointer.size() == 0) {
			free(subTree);
			return;
		}

		for (auto it = subTree->header_table_pointer.begin(); it != subTree->header_table_pointer.end(); it++) {
			buildSubTree(subTree, it->first);
		}
		free(subTree);
	}

#ifdef OUTPUT_ASCII
	char _format_char(unsigned int item) {
		return 'a' + item;
	}
#else
	unsigned int _format_char(unsigned int item) {
		return item;
	}
#endif

	// Debug
	void dumpTree(Tree *tree) {
		dfsPrintTree(tree->tree);
		cout << "----------" << endl;
	}
	void dfsPrintTree(TreeNode *&now) {
		for (auto iter = now->child.begin(); iter != now->child.end(); ++iter) {
			cout << now << "-";
			if (now->item == ROOT_VALUE) {
				cout << "ROOT";
			} else {
				cout << _format_char(now->item);
			}
			cout << "-" << now->count << " "
				 << iter->second << "-";
			if (iter->second == 0) {
				cout << 0 << "-" << 0 << endl;
			} else {
				cout << _format_char(iter->second->item) << "-" << iter->second->count << endl;
				dfsPrintTree(iter->second);
			}
		}
	}
	void dumpHeaderTable(Tree *tree) {
		for (auto iter = tree->header_table_pointer.begin(); iter != tree->header_table_pointer.end(); ++iter) {
			// start
			cout << _format_char(iter->first) << "-" << iter->second << " ";
			if (iter->second->start == nullptr) {
				cout << "NULL" << endl;
				continue;
			}
			cout << iter->second->start << "-" << _format_char(iter->second->start->item) << "-" << iter->second->start->count
				 << " start" << endl;

			// end
			cout << _format_char(iter->first) << "-" << iter->second << " "
				 << iter->second->end << "-" << _format_char(iter->second->end->item) << "-" << iter->second->end->count
				 << " end" << endl;
			auto now = iter->second->start;
			while (now != nullptr && now->next != nullptr) {
				cout << now << "-" << _format_char(now->item) << "-" << now->count << " "
					 << now->next << "-" << _format_char(now->next->item) << "-" << now->next->count << " next"
					 << endl;

				now = now->next;
			}
		}
		cout << "----------" << endl;
	}
};
