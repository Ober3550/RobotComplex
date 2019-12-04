#pragma once
#include <unordered_map>
#include <fstream>
#include <iostream>
#include "LogicTile.h"
#include "LogicTypes.h"

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
			while (!myfile.eof()) {
				myfile.read((char*)readMem, sizeof(keyType)+sizeof(valueType));
				this->insert({ readMem->first,readMem->second });
			}
			myfile.close();
		}
	}
	
};
template<>
class MyMap<uint64_t, LogicTile*> : public std::unordered_map<uint64_t, LogicTile*>
{
public:
	LogicTile** GetValue(uint64_t key)
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
				uint16_t* sectionSize = new uint16_t();
				*sectionSize = sizeof(*kv.second) - sizeof(uint32_t);
				myfile.write((char*)sectionSize, sizeof(uint16_t));
				//We don't need to write the key since it's just the encoded version of the position
				kv.second->Serialize(&myfile);
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
			uint16_t* sectionSize = new uint16_t();
			LogicTypes* newClass = new LogicTypes();
			uint64_t key;
			while (!myfile.eof()) {
				myfile.read((char*)sectionSize, sizeof(uint16_t));
				myfile.read((char*)newClass, sizeof(LogicTypes));
				LogicTile* newElement = LogicTile::Factory(*newClass);
				newElement->Deserialize(&myfile);
				key = newElement->pos.CoordToEncoded();
				this->insert({ key,newElement });
			}
			this->erase(key);
			myfile.close();
		}
	}
};