#pragma once
#include <vector>
#include "Pos.h"
struct RecipeComponent
{
	uint16_t itemTile;		// Item required
	uint8_t requirement;	// Quantity required
	int8_t resultState;		// Consumption or production after successful craft
};
class CraftingClass{
public:
	uint16_t recipeIndex;
	std::vector<RecipeComponent> recipe;
	uint8_t width;
	uint8_t height;
	uint32_t craftTicks;
	uint16_t animationReference;
	int CheckCrafting(Pos pos);
	void TryCraftingOther(uint16_t item, Pos itemPos); // Tries crafting a different recipe once the previous one has finished. For chaining processes
	void DoCrafting(Pos pos);
	void SuccessfulCraft(Pos pos);
	static void TryCrafting(uint16_t item, Pos itemPos);
};
