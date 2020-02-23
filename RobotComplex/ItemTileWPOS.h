#pragma once
#include "ParentTile.h"
struct ItemTileWPOS : public ParentTile
{
	Pos pos;
	uint16_t itemTile;
	uint8_t quantity;
	std::string GetTooltip();
	virtual void Serialize(std::ofstream*);
	virtual void Deserialize(std::ifstream*);
	virtual ItemTileWPOS* Copy();
};