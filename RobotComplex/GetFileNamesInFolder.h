#pragma once
#include <string>
#include <vector>
#include "Windows.h"
std::vector<std::string> getNamesInFolder(std::string folder);
std::vector<std::string> getFileNamesInFolder(std::string folder);
std::vector<std::string> getFolderNamesInFolder(std::string folder);