#pragma once
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <string>
#include <cerrno>
#include <stdexcept>
#include <sys/stat.h>
#include "LogicTile.h"
#include "LogicTypes.h"
#include "Robot.h"
#include "CraftingProcess.h"
#include "Pos.h"
#include "ItemTile.h"
#include "GetFileLength.h"
#ifndef __MYMAP_H__
#define __MYMAP_H__

template <typename keyType, typename valueType>
class MyMap : public std::unordered_map<keyType, valueType>
{
public:
	valueType* GetValue(keyType key)
	{
		auto find = this->find(key);
		return find != this->end() ? &(find->second) : nullptr;
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
		myfile.seekg(0, std::ios::beg);
		int fileLength = GetFileLength(filename);
		if (myfile.is_open() && fileLength > 0)
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

template <>
class MyMap<uint64_t, ItemTile> : public std::unordered_map<uint64_t, ItemTile>
{
public:
	ItemTile* GetValue(uint64_t key)
	{
		auto find = this->find(key);
		return find != this->end() ? &(find->second) : nullptr;
	}
	ItemTile* GetValue(Pos key)
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
			for (auto &kv : *this)
			{
				myfile.write((char*)&kv, sizeof(uint64_t) + sizeof(ItemTile));
			}
			myfile.close();
		}
	}
	void Deserialize(std::string filename, MyMap<uint16_t, uint16_t> newMap)
	{
		std::ifstream myfile;
		myfile.open(filename, std::ios::in | std::ios::binary);
		myfile.seekg(0, std::ios::beg);
		int fileLength = GetFileLength(filename);
		if (myfile.is_open() && fileLength > 0)
		{
			myfile.seekg(0, std::ios::beg);
			std::pair<uint64_t, ItemTile>* readMem = new std::pair<uint64_t, ItemTile>();
			while (!myfile.eof()) {
				myfile.read((char*)readMem, sizeof(uint64_t) + sizeof(ItemTile));
				if (uint16_t* newItemNum = newMap.GetValue(readMem->second.itemTile))
				{
					readMem->second.itemTile = *newItemNum;
					this->insert({ readMem->first,readMem->second });
				}
			}
			myfile.close();
		}
	}
};

template <>
class MyMap<uint64_t, Robot> : public std::unordered_map<uint64_t, Robot>
{
public:
	Robot* GetValue(uint64_t key)
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
				kv.second.Serialize(&myfile);
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
			Robot* readMem = new Robot();
			while (!myfile.eof()) {
				readMem->Deserialize(&myfile);
				this->insert({ readMem->pos.CoordToEncoded(),*readMem });
			}
			myfile.close();
		}
	}
};
template <>
class MyMap<uint64_t, CraftingProcess> : public std::unordered_map<uint64_t, CraftingProcess>
{
public:
	CraftingProcess* GetValue(uint64_t key)
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
				myfile.write((char*)&kv.second, sizeof(CraftingProcess));
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
			CraftingProcess* readMem = new CraftingProcess();
			while (!myfile.eof()) {
				myfile.read((char*)readMem, sizeof(CraftingProcess));
				this->insert({ readMem->pos.CoordToEncoded(),*readMem });
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
				// Section size is now stored separately
				//uint16_t* sectionSize = new uint16_t();
				//*sectionSize = sizeof(*kv.second) - sizeof(uint32_t);
				//myfile.write((char*)sectionSize, sizeof(uint16_t));
				//We don't need to write the key since it's just the encoded version of the position
				kv.second->Serialize(&myfile);
			}
			myfile.close();
		}
	}
	// We take in a new map incase of elements that were previously added get removed and the order of the elements change
	// We take in the memory size stored within the file so that we don't be gready when reading in memory
	void Deserialize(std::string filename, MyMap<uint8_t, uint8_t> newMap, std::vector<uint16_t> sizes)
	{
		std::ifstream myfile;
		myfile.open(filename, std::ios::in | std::ios::binary);
		myfile.seekg(0, std::ios::beg);
		int fileLength = GetFileLength(filename);
		if (myfile.is_open() && fileLength > 0)
		{
			myfile.seekg(0, std::ios::beg);
			//uint16_t* sectionSize = new uint16_t();
			LogicTypes* newClass = new LogicTypes();
			uint64_t prevKey;
			uint64_t key = 0;
			int successfulLoads = 0;
			while (!myfile.eof()) {
				prevKey = key;
				myfile.read((char*)newClass, sizeof(uint8_t));
				int elementSize = sizes[*newClass];
				if (uint8_t* newlogicType = newMap.GetValue(*newClass))
				{
					LogicTile* newElement = LogicTile::Factory((*newlogicType));
					newElement->Deserialize(&myfile, &elementSize);
					key = newElement->pos.CoordToEncoded();
					this->insert({ key,newElement });
					successfulLoads++;
				}
				else
				{
					LogicTile* failed = *this->GetValue(prevKey);
					delete failed;
				}
				// Otherwise element does not exist in the map
				// If memory block pointer isn't resting at the start of the next block move it forward or backward.
				if (elementSize != 0)
				{
					myfile.seekg(elementSize, std::ifstream::cur);
				}
			}
			this->erase(key);
			myfile.close();
		}
	}
};
#endif