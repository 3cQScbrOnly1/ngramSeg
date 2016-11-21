#include "load.h"
#include "seg.h"

#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
	myLoader loader;
	map<string, int> dic;
	loader.load_dic(argv[1], argv[2], dic);
	ifstream testFile(argv[3]);
	string line;
	map<string, double> unigram;
	loader.load_ngram(argv[4], unigram);
	Segmentor segmentor(argv[5]);
	while (getline(testFile, line))
	{
		if (line == "")
			continue;
		segmentor.createNodeNet(dic, line);
		segmentor.uniVTB(unigram);
	}
	testFile.close();
	return 0;
}