#pragma once
#include <unordered_map>
#include <fstream>
#include <iostream>

template <typename keyType, typename valueType>
class MyMap : public std::unordered_map<keyType, valueType>
{
public:
	valueType* GetValue(keyType key)
	{
		auto find = this->find(key);
		return find != this->end() ? &(find->second) : nullptr;
	}
	void Serialize(std::string filename)
	{
		std::ofstream myfile;
		myfile.open(filename, std::ios::out | std::ios::trunc | std::ios::binary);
		if (myfile.is_open())
		{
			for (auto &kv : *this)
			{
				myfile.write((char*)&kv, sizeof(keyType) + sizeof(valueType));
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
			std::pair<keyType,valueType>* readMem = new std::pair<keyType,valueType>();
			do{
				myfile.read((char*)readMem, sizeof(keyType)+sizeof(valueType));
				this->insert({ readMem->first,readMem->second });
			} while (!myfile.eof());
			myfile.close();
		}
	}
};