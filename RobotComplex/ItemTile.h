#include <cstdint>
#include "ParentTile.h"
#pragma once
struct ItemTile : public ParentTile
{
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
	virtual void Serialize(std::ofstream*);
	virtual void Deserialize(std::ifstream*);
};
