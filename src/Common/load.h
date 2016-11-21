#include <fstream>
#include <iostream>
#include <map>
#include <string>

using namespace std;


class myLoader{
public:
	void load_ngram(const char* ngram_path, map<string, double>& ngram)
	{
		ifstream ngram_file(ngram_path);
		string word;
		double prob;
		while (ngram_file >> word >> prob)
			ngram[word] = prob;
		ngram_file.close();
	}
	
	void load_dic(const char* dic_path, map<string, int>& dic)
	{
		ifstream dic_file(dic_path);
		string word;
		while (dic_file >> word)
			dic[word] = 0;
		dic_file.close();
	}

	void load_dic(const char* dic_path, const char* dic2_path, map<string, int>& dic)
	{
		ifstream dic_file(dic_path), dic2_file(dic2_path);
		string word;
		while (dic_file >> word)
			dic[word] = 0;
		while (dic2_file >> word)
			dic[word] = 0;
		dic_file.close();
		dic2_file.close();
	}

};