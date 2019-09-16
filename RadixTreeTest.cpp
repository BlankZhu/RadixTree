/*****************************************************************
** File : RadixTreeTest.cpp
** Description : An test of radix-tree
** Author : BlankZhu
*******************************************************************/
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "RadixTree.hpp"

void readfile(std::vector<std::string>& ret, const std::string & file_path)
{
	std::ifstream ifs(file_path);
	std::string line;
	while (std::getline(ifs, line))
	{
		boost::algorithm::trim(line);
		ret.push_back(line);
	}
	return;
}

int main(int argc, char* argv[])
{
	std::vector<std::string> svec{};
	RadixTree<std::string> tree;

	readfile(svec, "./vocabulary.txt");

	for (int i = 0; i < svec.size(); ++i)
	{
		tree.UpsertString(svec[i], svec[i] + "_value");
	}

	tree.DebugPrint();

	// std::cout << tree["a_word_here"] << '\n';

	return 0;
}
