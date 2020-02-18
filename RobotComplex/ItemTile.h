#include <cstdint>
#include "ParentTile.h"
#pragma once
struct ItemTile : public ParentTile
{
	uint16_t itemTile;
	uint8_t quantity;
};
