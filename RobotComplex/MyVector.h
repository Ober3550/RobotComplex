#pragma once
#include <vector>
#include <string>
#include <cerrno>
#include <stdexcept>
#include <sys/stat.h>

#include "GetFileLength.h"

template <typename valueType>
class MyVector : public std::vector<valueType>
{
public:
	valueType* GetValue(uint64_t key)
	{
		if (key > this->size())
			return nullptr;
		else
			return &(this->operator[](key));
	}
	valueType* GetValue(Pos key)
	{
		auto find = this->find(key.CoordToEncoded());
		return find != this->end() ? &(find->second) : nullptr;
	}
	void Serialize(std::string filename)
	{
		std::ofstream myfile;
		myfile.open(filename, std::ios::out | std::ios::trunc | std::ios::binary);
		if (myfile.is_open())
		{
			for (auto& value : *this)
			{
				myfile.write((char*)&value, sizeof(valueType));
			}
			myfile.close();
		}
	}
	void Deserialize(std::string filename)
	{
		std::ifstream myfile;
		myfile.open(filename, std::ios::in | std::ios::binary);
		myfile.seekg(0, std::ios::beg);
		int fileLength = GetFileLength(filename);
		if (myfile.is_open() && fileLength > 0)
		{
			myfile.seekg(0, std::ios::beg);
			valueType* readMem = new valueType();
			while (!myfile.eof()) {
				myfile.read((char*)readMem, sizeof(valueType));
				this->emplace_back(readMem);
			}
			myfile.close();
		}
	}
};


