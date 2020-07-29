#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include "Pos.h"
#include "CraftingClass.h"
#include "ItemTile.h"

class RecipeCompProto {
public:
	std::string itemName;
	int8_t resultState;
	uint8_t requirement = 1;
	operator RecipeComponent() const;
};
class RecipeProto {
public:
	std::vector<RecipeCompProto> recipe;
	uint8_t recipeWidth;					// Determines the shape of the recipe
	float craftingTime;						// Crafting time in seconds of the recipe
	std::string animation;					// Animation that plays when crafting
	RecipeProto(std::vector<RecipeCompProto> recipe, uint8_t recipeWidth, uint8_t craftingTime, std::string animation)
	{
		this->recipe = recipe;
		this->recipeWidth = recipeWidth;
		this->craftingTime = craftingTime;
		this->animation = animation;
	}
	RecipeProto()
	{
		this->recipe;
		this->recipeWidth = 1;
		this->craftingTime = 1;
		this->animation = "";
	}
};

class BigItem {
public:
	uint16_t	itemTile;
	int			quantity;
	operator ItemTile() const;
	BigItem() { this->itemTile = 0; this->quantity = 1; }
	BigItem(uint16_t itemTile, int quantity) { this->itemTile = itemTile; this->quantity = quantity; }
	BigItem(ItemTile item) { this->itemTile = item.itemTile; this->quantity = item.quantity; }
	BigItem(uint16_t item) { this->itemTile = item; this->quantity = 1; }
	std::string GetTooltip();
	void DrawItem(SpriteVector* appendTo, float x, float y, float s, uint8_t flags, sf::Color color);
};

class BigItemProto {
public:
	std::string itemName;
	int			quantity;
	operator BigItem() const;
};

class TechProto {
public:
	std::string name;
	std::string tips;
	std::vector<BigItemProto> requirements;
	std::vector<std::string>	 unlocks;
	bool unlocked = false;
	TechProto(std::string name, std::vector<BigItemProto> requirements, std::vector<std::string> unlocks)
	{
		this->requirements = requirements;
		this->unlocks = unlocks;
	}
	TechProto()
	{
		this->requirements;
		this->unlocks;
	}
	void Unlock();
	void Lock();
	void ShowRequirementsAsGrid();
};