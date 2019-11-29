#pragma once
#include "Pos.h"
class CraftingProcess {
public:
	Pos pos;
	uint16_t craftingRecipe;
	uint32_t ticks;
};