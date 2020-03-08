#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include "Pos.h"
#include "ElementTypes.h"
#include "LogicTypes.h"
#include "SpriteVector.h"
class ParentTile
{
public:
	virtual std::string GetTooltip() {
		return "";
	}
	virtual void Serialize(std::ofstream* myfile) {};
	virtual void Deserialize(std::ifstream* myfile) {};
	void DrawTile(SpriteVector* appendTo, float x, float y, float s, uint8_t flags, sf::Color color);
	static ParentTile* Factory(ElementTypes element, std::ifstream* myfile);
};
