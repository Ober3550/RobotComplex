#pragma once
#include <vector>
#include <string>
#include "MyMap.h"
template < typename T>
std::pair<bool, int > findInVector(const std::vector<T>& vecOfElements, const T& element)
{
	std::pair<bool, int > result;

	// Find given element in vector
	auto it = std::find(vecOfElements.begin(), vecOfElements.end(), element);

	if (it != vecOfElements.end())
	{
		result.first = true;
		result.second = distance(vecOfElements.begin(), it);
	}
	else
	{
		result.first = false;
		result.second = -1;
	}

	return result;
}
// Returns true if B is within A
