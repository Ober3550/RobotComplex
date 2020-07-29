#pragma once
#include "absl/container/flat_hash_map.h"
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
#include "Prototypes.h"
#include "KeyNames.h"
#include "MyStrings.h"

#ifndef __MYMAP_H__
#define __MYMAP_H__

template <typename keyType, typename valueType>
class MyMap : public absl::flat_hash_map<keyType,valueType>
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
				myfile.write((char*)&(kv.first), sizeof(keyType));
				myfile.write((char*)&(kv.second), sizeof(valueType));
			}
			myfile.close();
		}
	}
	void Deserialize(std::string filename)
	{
		std::ifstream myfile;
		myfile.open(filename, std::ios::in | std::ios::binary);
		int fileLength = GetFileLength(filename);
		if (myfile.is_open() && fileLength > 0)
		{
			myfile.seekg(0, std::ios::beg);
			keyType* key = new keyType();
			valueType* value = new valueType();
			while (!myfile.eof()) {
				myfile.read((char*)key, sizeof(keyType));
				myfile.read((char*)value, sizeof(valueType));
				this->insert({ *key, *value });
			}
			myfile.close();
		}
	}
};

template <typename keyType>
class MyMap<keyType, ItemTile> : public absl::flat_hash_map<keyType, ItemTile>
{
public:
	ItemTile* GetValue(keyType key)
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
				myfile.write((char*)&kv, sizeof(keyType) + sizeof(ItemTile));
			}
			myfile.close();
		}
	}
	void Deserialize(std::string filename, MyMap<uint16_t, uint16_t> newMap)
	{
		std::ifstream myfile;
		myfile.open(filename, std::ios::in | std::ios::binary);
		int fileLength = GetFileLength(filename);
		if (myfile.is_open() && fileLength > 0)
		{
			myfile.seekg(0, std::ios::beg);
			keyType* key = new keyType();
			ItemTile* value = new ItemTile();
			while (!myfile.eof()) {
				myfile.read((char*)key, sizeof(keyType));
				myfile.read((char*)value, sizeof(ItemTile));
				if (uint16_t* newItemNum = newMap.GetValue(value->itemTile))
				{
					value->itemTile = *newItemNum;
					this->insert({ *key, *value });
				}
			}
			myfile.close();
		}
	}
};

template <typename keyType>
class MyMap<keyType, BigItem> : public absl::flat_hash_map<keyType, BigItem>
{
public:
	BigItem* GetValue(keyType key)
	{
		auto find = this->find(key);
		return find != this->end() ? &(find->second) : nullptr;
	}
	BigItem* GetValue(Pos key)
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
			for (auto& kv : *this)
			{
				myfile.write((char*)&(kv.first), sizeof(keyType));
				myfile.write((char*)&(kv.second.itemTile), sizeof(uint16_t));
				myfile.write((char*)&(kv.second.quantity), sizeof(int));
			}
			myfile.close();
		}
	}
	void Deserialize(std::string filename, MyMap<uint16_t, uint16_t> newMap)
	{
		std::ifstream myfile;
		myfile.open(filename, std::ios::in | std::ios::binary);
		int fileLength = GetFileLength(filename);
		if (myfile.is_open() && fileLength > 0)
		{
			myfile.seekg(0, std::ios::beg);
			keyType* key = new keyType();
			BigItem* value = new BigItem();
			while (!myfile.eof()) {
				
				myfile.read((char*)key, sizeof(keyType));
				myfile.read((char*)&value->itemTile, sizeof(uint16_t));
				myfile.read((char*)&value->quantity, sizeof(int));
				if (uint16_t* newItemNum = newMap.GetValue(value->itemTile))
				{
					value->itemTile = *newItemNum;
					this->insert({ *key, *value });
				}
			}
			myfile.close();
		}
	}
};

template <>
class MyMap<sf::Event::KeyEvent, std::string> : public absl::flat_hash_map<sf::Event::KeyEvent, std::string>
{
public:
	std::string* GetValue(sf::Event::KeyEvent key)
	{
		auto find = this->find(key);
		return find != this->end() ? &(find->second) : nullptr;
	}
	void Serialize(std::string filename)
	{
		std::ofstream myfile(filename);
		for (auto& kv : *this)
		{
			myfile << KeyNames::toString(kv.first) << ':' << kv.second << '\n';
		}
	}
	void Deserialize(std::string filename)
	{
		std::ifstream myfile(filename);
		std::string str;
		while (std::getline(myfile, str)) {
			std::vector<std::string> splitStr;
			split(&splitStr, str, ':');
			if(splitStr.size() > 1)
				this->insert({ *KeyNames::toEvent(splitStr[0]),splitStr[1] });
		}
	}
};
#endif