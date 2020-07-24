#include "FindInVector.h"

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
std::vector<int> findSubstrings(MyMap<uint16_t, std::string>* stringMap, std::string key)
{
	std::vector<int> indexList;
	for (auto element : *stringMap)
	{
		if (containsSubstring(element.second, key))
			indexList.emplace_back(element.first);
	}
	return indexList;
}