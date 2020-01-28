#pragma once
#include <unordered_set>
#include <fstream>
#include <iostream>

template <typename keyType>
class MySet : public std::unordered_set<keyType>
{
public:
	bool contains(keyType key)
	{
		if (this->find(key) != this->end())
			return true;
		return false;
	}
	void Serialize(std::string filename)
	{
		std::ofstream myfile;
		myfile.open(filename, std::ios::out | std::ios::trunc | std::ios::binary);
		if (myfile.is_open())
		{
			for (auto &kv : *this)
			{
				myfile.write((char*)&kv, sizeof(this[0]));
			}
			myfile.close();
		}
	}
	void Deserialize(std::string filename)
	{
		std::ifstream myfile;
		myfile.open(filename, std::ios::in | std::ios::binary);
		if (myfile.is_open())
		{
			myfile.seekg(0, std::ios::beg);
			keyType* readMem = new keyType;
			do {
				myfile.read((char*)readMem, sizeof(this[0]));
				this->insert(*readMem);
			} while (!myfile.eof());
			myfile.close();
		}
	}
};