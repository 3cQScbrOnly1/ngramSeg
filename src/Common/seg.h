#include "Utf.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <map>

using namespace std;

class Node{
public:
	vector<Node*> pre_nodes;
	Node* best_pre;
	double best_score;

	int start;
	int length;
	string seg_words;
};

class Segmentor{
private:
	const static int max_seg = 7;
	vector<vector<Node*> > all_nodes;
	vector<Node*> end_nodes;
	ofstream result;


private:
	void createNodes(map<string, int>& dic, const string& sentence)
	{
		vector<string> all_words;
		map<string, int>::iterator it;
		getCharactersFromUTF8String(sentence, all_words);
		int offset;
		for (int i = 0; i < all_words.size(); i++)
		{
			vector<Node*> start_with_same_word_nodes;
			string words = "";
			for (int j = 0; j < max_seg; j++)
			{
				offset = i + j;
				if (offset < all_words.size())
				{
					words += all_words[i + j];
					it = dic.find(words);
					if (it != dic.end() || j == 0)
					{
						Node* the_node = new Node;
						the_node->start = i;
						the_node->length = j + 1;
						the_node->seg_words = words;
						start_with_same_word_nodes.push_back(the_node);
						if (offset == all_words.size() - 1)
							end_nodes.push_back(the_node);
					}
				}
			}
			all_nodes.push_back(start_with_same_word_nodes);
		}
	}

private:
	void showNode()
	{
		ofstream file("C:\\Users\\yunan\\Desktop\\log");
		for (int i = 0; i < all_nodes.size(); i++)
		{
			for (int j = 0; j < all_nodes[i].size(); j++)
			{
				file << "cur:\t" << all_nodes[i][j]->seg_words << " " << endl;
				file << "pre:\t" << " size:\t" << all_nodes[i][j]->pre_nodes.size() << endl;
				for (int k = 0; k < all_nodes[i][j]->pre_nodes.size(); k++)
					file << all_nodes[i][j]->pre_nodes[k]->seg_words << " ";
				file << endl;
			}
		}
		for (int i = 0; i < end_nodes.size(); i++)
			file << "end: "  << end_nodes[i]->seg_words << endl;
		file.close();
	}

private:
	void linkNodes()
	{
		int offset;
		for (int i = 0; i < all_nodes.size(); i++)
		{
			for (int j = 0; j < all_nodes[i].size(); j++)
			{
				offset = i + all_nodes[i][j]->length;
				if (offset < all_nodes.size())
				{
					for (int k = 0; k < all_nodes[offset].size(); k++)
					{
						all_nodes[offset][k]->pre_nodes.push_back(all_nodes[i][j]);
					}
				}
			}
		}
	}

public:
	void createNodeNet(map<string, int>& dic, const string& sentence)
	{
		clear();
		createNodes(dic, sentence);
		linkNodes();
		//showNode();
	}
private:
	void uniCalNode(map<string, double>& unigram)
	{
		double max_pre_score;
		Node* best_pre_node;
		map<string, double>::iterator it;
		for (int i = 0; i < all_nodes.size(); i++)
		{ 
			for (int j = 0; j < all_nodes[i].size(); j++)
			{
				it = unigram.find(all_nodes[i][j]->seg_words);
				if (all_nodes[i][j]->pre_nodes.size() == 0)
				{
					if (it != unigram.end())
					{
						all_nodes[i][j]->best_score = it->second;
					}
					else
					{
						all_nodes[i][j]->best_score = -1e10;
					}
					all_nodes[i][j]->best_pre = NULL;
				} 
				else
				{
					max_pre_score = all_nodes[i][j]->pre_nodes[0]->best_score;
					best_pre_node = all_nodes[i][j]->pre_nodes[0];
					for (int k = 1; k < all_nodes[i][j]->pre_nodes.size(); k++)
					{
						if (max_pre_score < all_nodes[i][j]->pre_nodes[k]->best_score)
						{
							max_pre_score = all_nodes[i][j]->pre_nodes[k]->best_score;
							best_pre_node = all_nodes[i][j]->pre_nodes[k];
						}
					}
					all_nodes[i][j]->best_pre = best_pre_node;
					if (it != unigram.end())
						all_nodes[i][j]->best_score = max_pre_score + it->second;
					else
						all_nodes[i][j]->best_score = max_pre_score + -1e10;
				}
			}
		}
	}
private:
	void biCalNode(map<string, double>& unigram, map<string, double>& bigram)
	{
		double cur_score, max_pre_score;
		Node* best_pre_node;
		string bi_word;
		map<string, double>::iterator it;
		for (int i = 0; i < all_nodes.size(); i++)
		{ 
			for (int j = 0; j < all_nodes[i].size(); j++)
			{
				if (all_nodes[i][j]->pre_nodes.size() == 0)
				{
					it = unigram.find(all_nodes[i][j]->seg_words);
					if (it != unigram.end())
						all_nodes[i][j]->best_score = it->second;
					else
						all_nodes[i][j]->best_score = -1e10;
					all_nodes[i][j]->best_pre = NULL;
				} 
				else
				{
					bi_word = all_nodes[i][j]->pre_nodes[0]->seg_words + "#" + all_nodes[i][j]->seg_words;
					it = bigram.find(bi_word);
					if (it == bigram.end())
						it = unigram.find(all_nodes[i][j]->seg_words);

					if (it == unigram.end())
						max_pre_score = all_nodes[i][j]->pre_nodes[0]->best_score + -1e10;
					else
						max_pre_score = all_nodes[i][j]->pre_nodes[0]->best_score + it->second;
					best_pre_node = all_nodes[i][j]->pre_nodes[0];
					for (int k = 1; k < all_nodes[i][j]->pre_nodes.size(); k++)
					{

						bi_word = all_nodes[i][j]->pre_nodes[k]->seg_words + "#" + all_nodes[i][j]->seg_words;
						it = bigram.find(bi_word);
						if (it == bigram.end())
							it = unigram.find(all_nodes[i][j]->seg_words);

						if (it == unigram.end())
							cur_score = all_nodes[i][j]->pre_nodes[k]->best_score + -1e10;
						else
							cur_score = all_nodes[i][j]->pre_nodes[k]->best_score + it->second;

						if (max_pre_score < cur_score)
						{
							max_pre_score = cur_score; 
							best_pre_node = all_nodes[i][j]->pre_nodes[k];
						}
					}
					all_nodes[i][j]->best_pre = best_pre_node;
					all_nodes[i][j]->best_score = max_pre_score;
				}
			}
		}
	}

private:
	void back()
	{
		Node* best_node = end_nodes[0];
		double best_score = end_nodes[0]->best_score;
		for (int i = 1; i < end_nodes.size(); i++)
		{
			if (best_score < end_nodes[i]->best_score)
			{
				best_node = end_nodes[i];
				best_score = end_nodes[i]->best_score;
			}
		}
		Node* pre_node = best_node->best_pre;
		stack<string> stack_words;
		stack_words.push(best_node->seg_words);
		
		while (pre_node != NULL)
		{
			stack_words.push(pre_node->seg_words);
			pre_node = pre_node->best_pre;
		}
		
		while (!stack_words.empty())
		{
			result << stack_words.top() << " ";
			stack_words.pop();
		}
		result << endl;
	}


public:
	void uniVTB(map<string, double>& unigram)
	{
		uniCalNode(unigram);
		back();
	}

public:
	void biVTB(map<string, double>& unigram, map<string, double>& bigram)
	{
		biCalNode(unigram, bigram);
		back();
	}
public:
	void clear()
	{
		for (int i = 0; i < all_nodes.size(); i++)
		{
			for (int j = 0; j < all_nodes[i].size(); j++)
			{
				if (all_nodes[i][j] != NULL)
					delete all_nodes[i][j];
			}
		}
		all_nodes.clear();
		end_nodes.clear();
	}

public:
	Segmentor()
	{}
public:
	Segmentor(const char* result_path)
	{
		result.open(result_path);
	}
public:
	~Segmentor()
	{
		for (int i = 0; i < all_nodes.size(); i++)
		{
			for (int j = 0; j < all_nodes[i].size(); j++)
			{
				if (all_nodes[i][j] != NULL)
					delete all_nodes[i][j];
			}
		}
		result.close();
	}
};