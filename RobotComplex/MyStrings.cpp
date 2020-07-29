#include "MyStrings.h"
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
bool containsSubstring(std::string stringA, std::string stringB)
{
	int length = (stringA.length() - stringB.length()) + 1;
	for (int i = 0; i < length; i++)
	{
		for (int j = 0; j < stringB.length(); j++)
		{
			if (j == stringB.length() - 1)
				return true;
			if (stringA[i + j] != stringB[j])
				break;
		}
	}
	return false;
}
std::vector<uint16_t> findSubstrings(MyMap<uint16_t, std::string>* stringMap, std::string key)
{
	std::vector<uint16_t> indexList;
	for (auto element : *stringMap)
	{
		if (containsSubstring(element.second, key))
			indexList.emplace_back(element.first);
	}
	return indexList;
}

std::string swapChar(std::string string, char charA, char charB)
{
	for (int i = 0; i < string.length(); i++)
	{
		if (string[i] == charA)
			string[i] = charB;
	}
	return string;
}

std::string capitalize(std::string string)
{
	if (string.length() == 0)
		return string;
	string[0] = std::toupper(string[0]);
	for (int i = 1; i < string.length(); i++)
	{
		if (string[i - 1] == ' ')
			string[i] = std::toupper(string[i]);
	}
	return string;
}

std::string lowercase(std::string string)
{
	for (int i = 0; i < string.length(); i++)
	{
		string[i] = std::tolower(string[i]);
	}
	return string;
}
namespace strings {
	void Serialize(std::vector<std::string>* vec, std::string filename)
	{
		std::ofstream myfile(filename);
		for (auto& value : *vec)
		{
			myfile << value << '\n';
		}
	}

	void Deserialize(std::vector<std::string>* vec, std::string filename)
	{
		std::ifstream myfile(filename);
		std::string str;
		while (std::getline(myfile, str)) {
			vec->emplace_back(str);
		}
	}
}
