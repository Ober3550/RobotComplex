#include "Tile.h"
#include "CraftingClass.h"
#include "Pos.h"
#include "WorldSave.h"
#include "ProgramData.h"
#include <unordered_set>
#include "CraftingSuccess.h"

int CraftingClass::CheckCrafting(Pos pos)
{
	int successReturn = craftagain;
	for (int l = 0; l < (int)recipe.size() / width; l++)
	{
		int row2 = l * width;
		for (int k = 0; k < width; k++)
		{
			Pos checkPos = pos.RelativePosition(k, l);
			RecipeComponent recipeComp = recipe[k + row2];
			if (recipeComp.itemTile != nothing) // If recipe component is nothing ground component doesn't matter
			{
				if (ItemTile * checkTile = world.GetItemTile(checkPos))
				{
					// Check if ground and recipe component are equal or if result will be populated at position
					if (checkTile->itemTile == recipeComp.itemTile)
					{
						if (recipeComp.resultState < 0)
						{
							int possibleCrafts = checkTile->quantity / recipeComp.requirement;
							if (possibleCrafts == 1)
								successReturn = success;
						}
					}
					else
						return failed;
				}
				else if (!(recipeComp.resultState > 0))	// If there is no item on ground but result state requires one
					return failed; // Ground component does not exist
			}
		}
	}
	return successReturn;
}
void CraftingClass::DoCrafting(Pos pos)
{
	auto tile = world.GetItemTile(pos);
	for (int j = 0; j < (int)recipe.size() / width; j++)
	{
		int row = j * width;
		for (int i = 0; i < width; i++)
		{
			// Find point of reference within recipe to start searching from. 
			if (recipe[i + row].itemTile == tile->itemTile)
			{
				if (CheckCrafting(pos.RelativePosition(-i, -j)))
				{
					CraftingProcess addProcess = { pos.RelativePosition(-i,-j), this->recipeIndex, craftTicks, craftTicks, this->animationReference };
					world.craftingQueue.insert({ pos.RelativePosition(-i,-j).CoordToEncoded(),addProcess });
				}
			}
		}
	}
}
void CraftingClass::TryCraftingOther(uint16_t item, Pos itemPos)
{
	if (auto recipeList = program.itemRecipeList.GetValue(item))
	{
		// Try to craft item when placed
		for (uint16_t recipeRef : *recipeList)
		{
			if(recipeRef != this->recipeIndex)
			program.craftingRecipes[recipeRef].DoCrafting(itemPos);
		}
	}
}
void CraftingClass::SuccessfulCraft(Pos pos)
{
	int success = CheckCrafting(pos);
	if (success)
	{
		for (int l = 0; l < (int)recipe.size() / width; l++)
		{
			int row2 = l * width;
			for (int k = 0; k < width; k++)
			{
				RecipeComponent recipeComp = recipe[k + row2];
				if (recipeComp.resultState != 0)
				{
					Pos alterPos = pos.RelativePosition(k, l);
					if (ItemTile * tile = &world.items[alterPos.CoordToEncoded()])
					{
						// This must be true otherwise the successful craft was false
						assert(world.ChangeItem(alterPos, recipeComp.itemTile, recipeComp.resultState));
						if (recipeComp.resultState > 0)	// If an item is populated at a tile
						{
							// Try to queue a different recipe according to that item type.
							TryCraftingOther(recipeComp.itemTile, alterPos);
						}
					}
				}
			}
		}
		if(success == craftagain)
		{
			world.craftingQueue[pos.CoordToEncoded()].ticks = craftTicks;
		}
	}
}

void CraftingClass::TryCrafting(uint16_t item, Pos itemPos)
{
	if (auto recipeList = program.itemRecipeList.GetValue(item))
	{
		// Try to craft item when placed
		for (uint16_t recipe : *recipeList)
		{
			program.craftingRecipes[recipe].DoCrafting(itemPos);
		}
	}
}