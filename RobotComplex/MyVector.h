#pragma once
#include <vector>
#include <string>
#include <cerrno>
#include <stdexcept>
#include <sys/stat.h>

#include "ParentTile.h"
#include "GetFileLength.h"
#include "ElementTypes.h"

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

template<>
class MyVector<ParentTile*> : public std::vector<ParentTile*>
{
public:
	ParentTile** GetValue(uint64_t key)
	{
		if (key > this->size())
			return nullptr;
		else
			return &(this->operator[](size_t(key)));
	}
	void Serialize(std::string filename)
	{
		std::ofstream myfile;
		myfile.open(filename, std::ios::out | std::ios::trunc | std::ios::binary);
		if (myfile.is_open())
		{
			for (auto& value : *this)
			{
				if (value)
				{
					ElementTypes* elementType = new ElementTypes();
					if (LogicTile* logic = dynamic_cast<LogicTile*> (value))
					{
						elementType = new ElementTypes(ElementTypes::logic);
					}
					else if (Robot* robot = dynamic_cast<Robot*> (value))
					{
						elementType = new ElementTypes(ElementTypes::robot);
					}
					else if (ItemTile* item = dynamic_cast<ItemTile*> (value))
					{
						elementType = new ElementTypes(ElementTypes::item);
					}
					if (elementType)
					{
						myfile.write((char*)elementType, sizeof(uint8_t));
						//We don't need to write the key since it's just the encoded version of the position
						value->Serialize(&myfile);
					}
				}
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
			ElementTypes* newClass = new ElementTypes();
			while (!myfile.eof()) {
				myfile.read((char*)newClass, sizeof(ElementTypes));
				ParentTile* newElement = ParentTile::Factory(*newClass, &myfile);
				if (newElement)
				{
					this->emplace_back(newElement);
				}
			}
			// Remove the last element cause it's usually junk
			delete this->back();
			this->pop_back();
			myfile.close();
		}
	}
};