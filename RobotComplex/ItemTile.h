#pragma once
#include <cstdint>
#include <string>
#include <iostream>
#include "SpriteVector.h"
class ItemTile
{
public:
	uint16_t itemTile;
	uint8_t quantity;
	ItemTile()
	{
		itemTile = 0;
		quantity = 1;
	}
	ItemTile(uint16_t item)
	{
		this->itemTile = item;
		this->quantity = 1;
	}
	ItemTile(uint16_t item, uint8_t quantity)
	{
		this->itemTile = item;
		this->quantity = quantity;
	}
	std::string GetTooltip();
	void DrawTile(SpriteVector* appendTo, float x, float y, float s, uint8_t flags, sf::Color color);
	virtual void Serialize(std::ofstream*);
	virtual void Deserialize(std::ifstream*);
};
