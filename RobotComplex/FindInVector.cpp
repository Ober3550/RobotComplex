#include "FindInVector.h"


std::vector<int> findSubstrings(const std::vector<std::string>* nameList, std::string key)
{
	std::vector<int> indexList;
	for (int i = 0; i < nameList->size(); i++)
	{
		std::string value = (*nameList)[i];
		if (value.find(key) != std::string::npos)
		{
			indexList.emplace_back(i);
		}
	}
	return indexList;
}