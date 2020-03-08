#include <cstdint>
#include "ParentTile.h"
#pragma once
class ItemTile : public ParentTile
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
	std::string GetTooltip();
	void DrawTile(SpriteVector* appendTo, float x, float y, float s, uint8_t flags, sf::Color color);
	virtual void Serialize(std::ofstream*);
	virtual void Deserialize(std::ifstream*);
};
