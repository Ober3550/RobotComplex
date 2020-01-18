#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include "Pos.h"
class RecipeCompProto {
public:
	std::string itemName;
	int8_t resultState;
	uint8_t requirement = 1;
};
class RecipeProto {
public:
	std::vector<RecipeCompProto> recipe;
	uint8_t recipeWidth;  // Determines the shape of the recipe
	uint8_t craftingTime; // Crafting time in seconds of the recipe
	std::string catalyst; // Item that when placed will activate the crafting
	std::string animation;// Animation that plays when crafting
	RecipeProto(std::vector<RecipeCompProto> recipe, uint8_t recipeWidth, uint8_t craftingTime, std::string catalyst, std::string animation)
	{
		this->recipe = recipe;
		this->recipeWidth = recipeWidth;
		this->craftingTime = craftingTime;
		this->catalyst = catalyst;
		this->animation = animation;
	}
};