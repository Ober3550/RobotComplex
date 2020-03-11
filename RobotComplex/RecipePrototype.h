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
	float craftingTime; // Crafting time in seconds of the recipe
	std::string animation;// Animation that plays when crafting
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