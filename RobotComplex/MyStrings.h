#pragma once
#include <vector>
#include <string>
void split(std::vector<std::string>* splitStr, std::string str, char delimeter);
bool containsSubstring(std::string stringA, std::string stringB);
std::string swapChar(std::string string, char A, char B);
std::string capitalize(std::string string);
std::string lowercase(std::string string);
namespace strings {
	void Serialize(std::vector<std::string>* vec, std::string filename);
	void Deserialize(std::vector<std::string>* vec, std::string filename);
}

