#include "SplitString.h"
void split(std::vector<std::string>* splitStr, std::string str, char delimeter)
{
	std::string word = "";
	for (size_t i=0;i<str.length();i++)
	{
		char c = str[i];
		if (c != delimeter)
			word += c;
		if (c == delimeter || i==str.length()-1)
		{
			if (word != "")
			{
				// Remove preceeding spaces
				while (word[0] == ' ')
				{
					word = word.substr(1, word.length());
				}
				// Remove following spaces
				while(word[word.length() - 1] == ' ')
				{
					word = word.substr(0, word.length() - 1);
				}
				splitStr->emplace_back(word);
				word = "";
			}
		}
	}
}