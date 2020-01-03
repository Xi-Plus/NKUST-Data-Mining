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
typedef vector<double> ID3DATAROW;   // ID3 單行資料 (一維陣列)
typedef vector<ID3DATAROW> ID3DATA;  // ID3 資料 (二維陣列)

class ID3 {
   private:
	struct TreeNode {			   // ID3 樹結構
		unsigned int splitby;	  // 記錄由哪個屬性作分類節點
		double splitarg;		   // 記錄由該屬性的哪個門檻值作分類節點
		vector<TreeNode *> child;  // 子樹
	};

	// Arguments
	char inputpath[100], outputpath[100];
	fstream fin, fout;
	// Data
	vector<string> column;				 // 屬性名稱
	unsigned int columnsize;			 // 屬性數量 (含class)
	unordered_map<string, int> str2int;  // 將 class 名稱編碼成數值，方便運算
	unordered_map<int, string> int2str;  // 將 class 代號解碼成名稱
	TreeNode *root;						 // ID3 樹根節點
	// Temp
	char temps[100];

   public:
	// 初始化 ID3
	ID3(char *_inputpath, char *_outputpath) {
		strcpy(inputpath, _inputpath);
		strcpy(outputpath, _outputpath);
	}
	// 主函數
	void run() {
		cout.setf(ios::unitbuf);

		// 設定輸入路徑
		fin.open(inputpath, fstream::in | fstream::binary);
		if (!fin.is_open()) {
			cerr << "Fail to open input file";
			exit(1);
		}

		// 設定輸出路徑
		fout.open(outputpath, fstream::out);
		if (!fout.is_open()) {
			cerr << "Fail to open output file";
			exit(1);
		}

		string s;
		getline(fin, s);
		stringstream ss(s);

		// 讀取 CSV 檔標頭 (屬性名稱)
		while (getline(ss, s, ',')) {
			column.push_back(s);  // 記錄屬性名
			cout << s << endl;
		}
		columnsize = column.size();  // 屬性個數
		cout << columnsize << endl;

		ID3DATA data;  // ID3 資料
		double d = 0;
		char c;

		// 讀取每行資料
		while (getline(fin, s)) {
			stringstream ss2(s);
			ID3DATAROW tempdata;  // ID3 單行資料

			// 讀取單行資料
			for (int q = 0; q < columnsize - 1; q++) {
				ss2 >> d;
				ss2 >> c;
				// cout << d << c << "\t";
				tempdata.push_back(d);  // 記錄資料
			}
			ss2 >> s;
			// cout << "*" << s << "*" << endl;
			tempdata.push_back(getindex(s));  // 該資料的分類結果
			data.push_back(tempdata);		  // 將單行資料放入所有資料
		}
		// for (auto v : data) {
		// 	for (auto v2 : v) {
		// 		cout << v2 << " ";
		// 	}
		// 	cout << endl;
		// }
		root = new TreeNode();  // 初始化 ID3 樹根節點
		dfssplit(root, data);   // 遞迴產生 ID3 樹
		cout << "Done" << endl;
		dfsprint(root, "");  // 繪製 ID3 樹

		fout.close();
	}

	// 編碼分類名稱
	int getindex(string data) {
		// 如果已編碼過，直接回傳編碼值
		if (str2int[data] != 0) {
			return str2int[data];
		}

		// 給予新的編碼值
		str2int[NEXT_INDEX_KEY]++;
		str2int[data] = str2int[NEXT_INDEX_KEY];
		int2str[str2int[NEXT_INDEX_KEY]] = data;
		return str2int[NEXT_INDEX_KEY];
	}

	/*
	 * 產生 ID3 樹
	 * root: 當前的 ID3 樹節點
	 * data: 當前的資料集
	 */
	void dfssplit(TreeNode *root, ID3DATA data) {
		vector<ID3DATA> ansdatas;  // 保存分類後的資料

		double nowEntropy = calcEntropy(data);		  // 計算目前的 entropy
		if (nowEntropy < 1e-6) {					  // 如果 entropy 等於 0，此用來防止浮點誤差
			root->splitby = data[0][columnsize - 1];  // 則記錄分類答案
			return;									  // entropy = 0 ，無需再分類
		}

		// 開始分類
		double minEntropy = 1e9;
		unsigned int minSplit;
		double minSplitArg;
		// cout << nowEntropy << endl;

		// 嘗試以每個屬性進行分類
		for (unsigned int trysplit = 0; trysplit < columnsize - 1; trysplit++) {
			set<double> usedarg;  // 防止計算重複屬性值來增快速度

			// 嘗試以該屬性的每個屬性值作為門檻分類
			for (int i = data.size() - 1; i >= 0; i--) {
				double trysplitarg = data[i][trysplit];  // 要分類的門檻值
				if (usedarg.count(trysplitarg)) {		 // 如果已計算過則略過
					continue;
				}

				vector<ID3DATA> tempdatas;  // 暫存分類後資料
				tempdatas.resize(2);		// 分類後資料都是2個，小於等於門檻 和 大於門檻
				// cout << "len " << str2int[trysplit][NEXT_INDEX_KEY] << endl;
				for (auto &d : data) {
					if (d[trysplit] <= trysplitarg) {  //小於等於門檻
						tempdatas[0].push_back(d);
					} else {  // 大於門檻
						tempdatas[1].push_back(d);
					}
				}

				// 計算新的 entropy
				double newEntropy = 0;
				for (auto &ds : tempdatas) {
					newEntropy += calcEntropy(ds) * ds.size() / data.size();
				}

				// cout << trysplit << " " << newEntropy << endl;

				// 如果新的 entropy 較低，則把目前的分類狀態覆蓋目前答案
				if (newEntropy <= minEntropy) {
					minEntropy = newEntropy;
					ansdatas = tempdatas;
					minSplit = trysplit;
					minSplitArg = trysplitarg;
				}
				// else if (abs(newEntropy - minEntropy) < 1e-6) {
				// 	cout<<"same entropy "<<trysplit<<" "<<trysplitarg<<" "<<newEntropy<<endl;
				// }
				usedarg.insert(trysplitarg);  // 記錄已計算過的門檻值
			}
		}
		// cout << minEntropy << " " << minSplit << endl;

		// 記錄真正的答案到 ID3 樹的節點上
		root->splitby = minSplit;
		root->splitarg = minSplitArg;
		// cout<<"Split by "<<minSplit<<" "<<minSplitArg<<" entropy "<<minEntropy<<endl;

		root->child.resize(ansdatas.size());
		for (unsigned int key = 0; key < ansdatas.size(); key++) {
			if (ansdatas[key].size() > 0) {
				// 產生新的子樹，並把分類後的子資料繼續進行分類
				root->child[key] = new TreeNode();
				dfssplit(root->child[key], ansdatas[key]);
			}
		}
	}

	// 計算 entropy
	double calcEntropy(ID3DATA data) {
		unordered_map<unsigned int, unsigned int> count;
		double entropy = 0;
		// 計算各分類出現次數
		for (auto row : data) {
			count[row[columnsize - 1]]++;
		}
		// 按照分類數量計算 entropy
		for (auto it = count.begin(); it != count.end(); it++) {
			entropy += -(1.0 * it->second / data.size()) * log2(1.0 * it->second / data.size());
			// cout << it->first << " " << it->second << endl;
		}
		return entropy;
	}

	// 使用 DFS 遞迴繪製 ID3 樹
	void dfsprint(TreeNode *root, string prefix) {
		if (root->child.size() == 0) {
			// 已分類完畢的答案
			sprintf(temps, "%s\n",
					strNorm(int2str[root->splitby]).c_str());
			fout << temps;
			return;
		} else {
			// 分類屬性
			sprintf(temps, "%s\n",
					strNorm(column[root->splitby]).c_str());
			fout << temps;
		}

		// 一些美化輸出的計算
		int lastchild = root->child.size() - 1;
		while (lastchild >= 0 && root->child[lastchild] == nullptr) {
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
				fout << root->splitarg << " ──";  // 分類門檻
				dfsprint(root->child[q], prefix + (q == lastchild ? "    " : "|   "));
			}
		}
	}

	// 將屬性名的空格換成底線
	string strNorm(string s) {
		for (int q = s.size() - 1; q >= 0; q--) {
			if (s[q] == ' ') {
				s[q] = '_';
			}
		}
		return s;
	}
};
