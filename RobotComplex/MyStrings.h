#pragma once
#include <vector>
#include <string>
#include "MyMap.h"
void split(std::vector<std::string>* splitStr, std::string str, char delimeter);
bool containsSubstring(std::string stringA, std::string stringB);
std::vector<uint16_t> findSubstrings(MyMap<uint16_t, std::string>* stringMap, std::string element);
std::string swapChar(std::string string, char A, char B);
std::string capitalize(std::string string);
std::string lowercase(std::string string);
namespace strings {
	void Serialize(std::vector<std::string>* vec, std::string filename);
	void Deserialize(std::vector<std::string>* vec, std::string filename);
}

