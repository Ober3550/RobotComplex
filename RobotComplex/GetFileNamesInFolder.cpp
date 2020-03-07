#include "GetFileNamesInFolder.h"
std::vector<std::string> getNamesInFolder(std::string folder){
	std::vector<std::string> names;
	std::string pattern(folder);
	pattern.append("\\*");
	WIN32_FIND_DATA data;
	HANDLE hFind;
	if ((hFind = FindFirstFile(pattern.c_str(), &data)) != INVALID_HANDLE_VALUE) {
		do {
			names.push_back(data.cFileName);
		} while (FindNextFile(hFind, &data) != 0);
		FindClose(hFind);
	}
	names.erase(names.begin()); // Removes . filesystem element
	names.erase(names.begin()); // Removes .. filesystem element
	return names;
}
std::vector<std::string> getFileNamesInFolder(std::string folder)
{
	std::vector<std::string> names = getNamesInFolder(folder);
	for (std::vector<std::string>::iterator iter = names.begin(); iter < names.end(); iter++)
	{
		std::string name = *iter;
		if (name.length() > 3)
		{
			if (name[name.length() - 4] != '.')
				iter = names.erase(iter);
		}
		else
			iter = names.erase(iter);
	}
	return names;
}
std::vector<std::string> getFolderNamesInFolder(std::string folder)
{
	std::vector<std::string> names = getNamesInFolder(folder);
	for (std::vector<std::string>::iterator iter = names.begin(); iter < names.end();iter++)
	{
		std::string name = *iter;
		if (name.length() > 3)
		{
			if (name[name.length() - 4] == '.')
				iter = names.erase(iter);
		}
	}
	return names;
}