#pragma once
#include <vector>
#include <string>
void split(std::vector<std::string>* splitStr, std::string str, char delimeter)
{
	std::string word = "";
	for (char c : str)
	{
		if (c == delimeter)
		{
			if(word != "")
			splitStr->emplace_back(word);
			word = "";
		}
		else
		{
			if(c != ' ')
			word += c;
		}
	}
	if(word != "")
	splitStr->emplace_back(word);
}