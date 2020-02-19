#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include "Pos.h"
#include "ElementTypes.h"
#include "LogicTypes.h"
class ParentTile
{
public:
	virtual std::string GetTooltip() {
		return "";
	}
	virtual void Serialize(std::ofstream* myfile) {};
	virtual void Deserialize(std::ifstream* myfile) {};
	static ParentTile* Factory(ElementTypes element, std::ifstream* myfile);
};
