#include <cassert>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <iostream>
#include <string>
#include "id3.cpp"

using namespace std;

int main(int argc, char *argv[]) {
	char inputpath[100], outputpath[100];
	if (argc < 3) {
		printf("Missing parameter\n");
		string temps;
		printf("file input:");
		getline(cin, temps);
		strcpy(inputpath, temps.c_str());
		printf("file output: ");
		getline(cin, temps);
	} else {
		strcpy(inputpath, argv[1]);
		strcpy(outputpath, argv[2]);
	}
	printf("file input: %s\n", inputpath);
	printf("file output: %s\n", outputpath);

	clock_t start_time = clock();

	ID3 id3(inputpath, outputpath);
	id3.run();

	printf("It tooks %ld milliseconds\n", (clock() - start_time) * 1000 / CLOCKS_PER_SEC);

	return 0;
}
