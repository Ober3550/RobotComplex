#pragma once
#include <unordered_map>
template <typename keyType, typename valueType>
class MyMap : public std::unordered_map<keyType, valueType>
{
public:
	valueType* GetValue(keyType key)
	{
		auto find = this->find(key);
		return find != this->end() ? &(find->second) : nullptr;
	}
};