#include "Prototypes.h"
#include "FindInVector.h"
#include "ProgramData.h"
#include "WorldSave.h"
#include "windows.h"

void TechProto::Unlock()
{
	this->unlocked = true;
	for (auto unlock : unlocks)
	{
		auto recipe = program.recipeNameToIndex.find(unlock);
		if (recipe != program.recipeNameToIndex.end())
		{
			program.craftingRecipes[recipe->second].unlocked = true;
		}
	}
}

void TechProto::Lock()
{
	this->unlocked = false;
	for (auto unlock : unlocks)
	{
		auto recipe = program.recipeNameToIndex.find(unlock);
		if (recipe != program.recipeNameToIndex.end())
		{
			program.craftingRecipes[recipe->second].unlocked = false;
		}
	}
}

RecipeCompProto::operator RecipeComponent() const {
	RecipeComponent recipe;
	auto lookup = program.itemLookups.find(this->itemName);
	if (lookup != program.itemLookups.end())
	{
		recipe.itemTile = program.itemLookups[this->itemName];
		recipe.requirement = this->requirement;
		recipe.resultState = this->resultState;
	}
	else
	{
		OutputDebugStringA(std::string("Error: " + this->itemName + " is not a valid item name").c_str());
		return recipe;
	}
}

BigItemProto::operator BigItem() const {
	BigItem item;
	auto lookup = program.itemLookups.find(this->itemName);
	if (lookup != program.itemLookups.end())
	{
		item.itemTile = program.itemLookups[this->itemName];
		item.quantity = this->quantity;
	}
	else
	{
		OutputDebugStringA(std::string("Error: " + this->itemName + " is not a valid item name").c_str());
	}
	return item;
}

std::string BigItem::GetTooltip() { return ItemTile(*this).GetTooltip(); }
void BigItem::DrawItem(SpriteVector* appendTo, float x, float y, float s, uint8_t flags, sf::Color color) { ItemTile(*this).DrawItem(appendTo, x, y, s, flags, color); }

BigItem::operator ItemTile() const {
	ItemTile item;
	item.itemTile = this->itemTile;
	item.quantity = uint8_t(this->quantity);
	return item;
}

void TechProto::ShowRequirementsAsGrid()
{
	program.technologyView.clear();
	int width = std::max(requirements.size(), unlocks.size());
	program.technologyViewSize = {uint8_t(width * 2 + 1), 1 };
	bool completedTech = true;
	for (int i = 0; i < requirements.size(); i++)
	{
		BigItem recipe = requirements[i];
		if (recipe.quantity - world.resourcesDelivered[recipe.itemTile] > 0)
		{
			recipe.quantity -= world.resourcesDelivered[recipe.itemTile];
			completedTech = false;
		}
		else
		{
			recipe.quantity = 0;
		}
		if(recipe.itemTile)
			program.technologyView.insert({ SmallPos{uint8_t(i), 0}, recipe });
	}
	if (completedTech)
	{
		Unlock();
		program.technologyView.clear();
		world.FindNextTechnology();
		return;
	}
	for (int i=0;i<unlocks.size();i++)
	{
		std::string unlock = unlocks[i];
		ItemTile item = ItemTile(program.itemLookups[unlock]);
		program.technologyView.insert({ SmallPos{uint8_t(width + 1 + i), 0}, item });
	}
}