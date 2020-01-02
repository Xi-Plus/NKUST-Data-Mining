#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <unordered_map>
#include <vector>

using namespace std;

#define NEXT_INDEX_KEY ""
typedef vector<double> ID3DATAROW;
typedef vector<ID3DATAROW> ID3DATA;

class ID3 {
   private:
	struct TreeNode {
		unsigned int splitby;  // or answer
		double splitarg;
		vector<TreeNode *> child;
	};
	// Arguments
	char inputpath[100], outputpath[100];
	fstream fin, fout;
	// Data
	vector<string> column;
	unsigned int columnsize;
	unordered_map<string, int> str2int;
	unordered_map<int, string> int2str;
	TreeNode *root;
	// Temp
	char temps[100];

   public:
	ID3(char *_inputpath, char *_outputpath) {
		strcpy(inputpath, _inputpath);
		strcpy(outputpath, _outputpath);
	}
	void run() {
		cout.setf(ios::unitbuf);

		fin.open(inputpath, fstream::in | fstream::binary);
		if (!fin.is_open()) {
			cerr << "Fail to open input file";
			exit(1);
		}

		fout.open(outputpath, fstream::out);
		if (!fout.is_open()) {
			cerr << "Fail to open output file";
			exit(1);
		}

		string s;
		getline(fin, s);
		stringstream ss(s);

		while (getline(ss, s, ',')) {
			column.push_back(s);
			cout << s << endl;
		}
		columnsize = column.size();
		cout << columnsize << endl;

		ID3DATA data;
		double d=0;
		char c;
		int tt;

		while (getline(fin, s)) {
			stringstream ss2(s);
			ID3DATAROW tempdata;
			for (int q = 0; q < columnsize - 1; q++) {
				ss2 >> d;
				ss2 >> c;
				// cout << d << c << "\t";
				tempdata.push_back(d);
			}
			ss2 >> s;
			// cout << "*" << s << "*" << endl;
			tempdata.push_back(getindex(s));
			data.push_back(tempdata);
		}
		// for (auto v : data) {
		// 	for (auto v2 : v) {
		// 		cout << v2 << " ";
		// 	}
		// 	cout << endl;
		// }
		root = new TreeNode();
		dfssplit(root, data);
		cout<<"Done"<<endl;
		dfsprint(root, "");

		fout.close();
	}

	int getindex(string data) {
		if (str2int[data] != 0) {
			return str2int[data];
		}
		str2int[NEXT_INDEX_KEY]++;
		str2int[data] = str2int[NEXT_INDEX_KEY];
		int2str[str2int[NEXT_INDEX_KEY]] = data;
		return str2int[NEXT_INDEX_KEY];
	}

	void dfssplit(TreeNode *root, ID3DATA data) {
		vector<ID3DATA> ansdatas;

		double nowEntropy = calcEntropy(data);
		if (nowEntropy < 1e-6) {
			root->splitby = data[0][columnsize - 1]; // answer
			return;
		}
		double minEntropy = 1e9;
		unsigned int minSplit;
		double minSplitArg;
		// cout << nowEntropy << endl;
		for (unsigned int trysplit = 0; trysplit < columnsize - 1; trysplit++) {
			set<double> usedarg;
			for (int i = data.size()-1; i >=0; i--) {
				double trysplitarg = data[i][trysplit];
				if (usedarg.count(trysplitarg)) {
					continue;
				}

				vector<ID3DATA> tempdatas;
				tempdatas.resize(2);
				// cout << "len " << str2int[trysplit][NEXT_INDEX_KEY] << endl;
				for (auto &d : data) {
					if (d[trysplit] <= trysplitarg) {
						tempdatas[0].push_back(d);
					} else {
						tempdatas[1].push_back(d);
					}
				}
				double newEntropy = 0;
				for (auto &ds : tempdatas) {
					newEntropy += calcEntropy(ds) * ds.size() / data.size();
				}
				// cout << trysplit << " " << newEntropy << endl;
				if (newEntropy <= minEntropy) {
					minEntropy = newEntropy;
					ansdatas = tempdatas;
					minSplit = trysplit;
					minSplitArg = trysplitarg;
				}
				// else if (abs(newEntropy - minEntropy) < 1e-6) {
				// 	cout<<"same entropy "<<trysplit<<" "<<trysplitarg<<" "<<newEntropy<<endl;
				// }
				usedarg.insert(trysplitarg);
			}
		}
		// cout << minEntropy << " " << minSplit << endl;
		root->splitby = minSplit;
		root->splitarg = minSplitArg;
		// cout<<"Split by "<<minSplit<<" "<<minSplitArg<<" entropy "<<minEntropy<<endl;
		root->child.resize(ansdatas.size());
		for (unsigned int key = 0; key < ansdatas.size(); key++) {
			if (ansdatas[key].size() > 0) {
				root->child[key] = new TreeNode();
				dfssplit(root->child[key], ansdatas[key]);
			}
		}
	}

	double calcEntropy(ID3DATA data) {
		unordered_map<unsigned int, unsigned int> count;
		double entropy = 0;
		for (auto row : data) {
			count[row[columnsize - 1]]++;
		}
		for (auto it = count.begin(); it != count.end(); it++) {
			entropy += -(1.0 * it->second / data.size()) * log2(1.0 * it->second / data.size());
			// cout << it->first << " " << it->second << endl;
		}
		return entropy;
	}

	void dfsprint(TreeNode *root, string prefix) {
		if (root->child.size() == 0) {
			sprintf(temps, "%s\n",
					strNorm(int2str[root->splitby]).c_str());
			fout << temps;
			return;
		} else {
			sprintf(temps, "%s\n",
					strNorm(column[root->splitby]).c_str());
			fout << temps;
		}
		int lastchild = root->child.size() - 1;
		while (lastchild >= 0 &&root->child[lastchild] == nullptr){
			lastchild--;
		}
		if (lastchild == -1) {
			return;
		}

		for (int q = 0; q < root->child.size(); q++) {
			if (root->child[q] != nullptr) {
				sprintf(temps, "%s%s── ",
						prefix.c_str(),
						(q == lastchild ? "└" : "├"));
				fout << temps;
				if (q == 0) {
					fout << "<=";
				} else {
					fout << ">";
				}
				fout << root->splitarg << " ──";
				dfsprint(root->child[q], prefix + (q == lastchild ? "    " : "|   "));
			}
		}
	}

	string strNorm(string s) {
		for (int q = s.size() - 1; q >= 0; q--) {
			if (s[q] == ' ') {
				s[q] = '_';
			}
		}
		return s;
	}
};
