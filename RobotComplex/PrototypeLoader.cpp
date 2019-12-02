#include <SFML/Graphics.hpp>
#include "WorldSave.h"
#include "ProgramData.h"
#include "Pos.h"
#include "Windows.h"
#include "LogicTile.h"
#include <string>
#include "MyMod.h"
#include "CraftingProcess.h"
#include "RedirectorColors.h"
#include "SpriteGenerator.h"
#include "TestWorld.h"
#include "PrototypeLoader.h"
#include "RecipePrototype.h"

// Central function for error checking on texture loading
sf::Texture LoadTexture(std::string filename)
{
	sf::Texture texture;
	if (!texture.loadFromFile("Assets/x32/" + filename))
	{
		OutputDebugStringA((filename + " failed to load\r\n").c_str());
	}
	return texture;
}

sf::Texture LogicTile::texture = LoadTexture("blank.png");
sf::Texture Wire::texture = LoadTexture("logic/wire.png");
sf::Texture PressurePlate::texture = LoadTexture("logic/pressureplate.png");
sf::Texture Redirector::texture = LoadTexture("logic/redirector.png");
sf::Texture Inverter::texture = LoadTexture("logic/inverter.png");
sf::Texture Booster::texture = LoadTexture("logic/inverter.png");
sf::Texture Repeater::texture = LoadTexture("logic/inverter.png");
sf::Texture Holder::texture = LoadTexture("logic/holder.png");
sf::Texture Memory::texture = LoadTexture("logic/memory.png");
sf::Texture Counter::texture = LoadTexture("logic/counter.png");

void LoadAllTextures()
{
	// Load Robot Texture
	program.robotTexture = LoadTexture("robotNew.png");
	program.font = LoadTexture("font.png");

	std::string fontIndex = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ.";
	for (int i = 0; i < (int)fontIndex.length(); i++)
	{
		sf::IntRect fontRect = sf::IntRect(i * 3, 0, 3, 5);
		program.fontMap.insert({ fontIndex[i], fontRect });
	}
	if (!program.guiFont.loadFromFile("hemi head bd it.ttf"))
	{
		//error
	}
	program.buttonTexture = LoadTexture("button.png");
}

template < typename T>
std::pair<bool, int > findInVector(const std::vector<T>& vecOfElements, const T& element)
{
	std::pair<bool, int > result;

	// Find given element in vector
	auto it = std::find(vecOfElements.begin(), vecOfElements.end(), element);

	if (it != vecOfElements.end())
	{
		result.second = distance(vecOfElements.begin(), it);
		result.first = true;
	}
	else
	{
		result.first = false;
		result.second = -1;
	}

	return result;
}

void LoadPrototypes()
{
	LoadAllTextures();

	// Ground prototypes
	std::vector<std::string> groundPrototypes;
	groundPrototypes.emplace_back("grass");
	groundPrototypes.emplace_back("sand");
	groundPrototypes.emplace_back("water");

	for (const std::string& text : groundPrototypes)
	{
		program.groundTextures.emplace_back(LoadTexture("ground/" + text + ".png"));
	}

	// Item prototypes
	std::vector<std::string> itemTooltips;
	std::vector<std::string> itemPrototypes;
	itemPrototypes.emplace_back("nothing");
	itemTooltips.emplace_back("Nothing");
	itemPrototypes.emplace_back("anything");
	itemTooltips.emplace_back("Anything");
	itemPrototypes.emplace_back("energy");
	itemTooltips.emplace_back("");
	itemPrototypes.emplace_back("coal_ore");
	itemTooltips.emplace_back("Coal ore");
	itemPrototypes.emplace_back("iron_ore");
	itemTooltips.emplace_back("Iron ore");
	itemPrototypes.emplace_back("copper_ore");
	itemTooltips.emplace_back("Copper ore");
	itemPrototypes.emplace_back("iron_ingot");
	itemTooltips.emplace_back("Iron ingot");
	itemPrototypes.emplace_back("copper_ingot");
	itemTooltips.emplace_back("Copper ingot");
	itemPrototypes.emplace_back("clay");
	itemTooltips.emplace_back("Clay");
	itemPrototypes.emplace_back("clay_brick");
	itemTooltips.emplace_back("Clay Brick");

	for (const std::string& text : itemPrototypes)
	{
		program.itemTextures.emplace_back(LoadTexture("items/" + text + ".png"));
	}
	for (const std::string& text : itemTooltips)
	{
		program.itemTooltips.emplace_back(text);
	}

	//Logic Tooltips
	std::vector<std::string> logicTypeTooltips;
	program.logicTooltips.emplace_back(logicTypeTooltips);	// Wire
	program.logicTooltips.emplace_back(logicTypeTooltips);	// Redirector
	program.logicTooltips.emplace_back(logicTypeTooltips);  // Pressureplate
	program.logicTooltips.emplace_back(logicTypeTooltips);	// Inverter
	program.logicTooltips.emplace_back(logicTypeTooltips);	// Booster
	program.logicTooltips.emplace_back(logicTypeTooltips);	// Repeater
	program.logicTooltips.emplace_back(logicTypeTooltips);	// Memory
	program.logicTooltips.emplace_back(logicTypeTooltips);	// Holder
	program.logicTooltips.emplace_back(logicTypeTooltips);	// Counter
	program.logicTooltips[0].emplace_back("Wire transfers signals");
	program.logicTooltips[1].emplace_back("Does nothing when powered");
	program.logicTooltips[1].emplace_back("Redirects and makes robot drop it's item");
	program.logicTooltips[1].emplace_back("Redirects when robot's holding ");
	program.logicTooltips[2].emplace_back("Creates signal when robot or item ontop");
	program.logicTooltips[3].emplace_back("Inverter: output = max - input");
	program.logicTooltips[4].emplace_back("Gives out max signal for any input above zero");
	program.logicTooltips[5].emplace_back("Gives out same signal as input");
	program.logicTooltips[6].emplace_back("Stores 16 different values that cycle when input rises to high");
	program.logicTooltips[7].emplace_back("Catches robots and releases when powered");
	program.logicTooltips[8].emplace_back("Counts up to the max value then resets");

	// Animation prototypes
	std::vector<std::string> animationPrototypes;
	animationPrototypes.emplace_back("furnace_animation");

	for (const std::string& text : animationPrototypes)
	{
		program.animationTextures.emplace_back(LoadTexture("machines/" + text + ".png"));
	}

	// Recipe Prototypes: Recipe, Recipe Width, Craft Time, Recipe Catalyst, Animation, Animation Offset
	std::vector<RecipeProto> recipePrototypes;
	recipePrototypes.emplace_back(RecipeProto({ { "iron_ingot"  ,1 }, { "iron_ore",  -1 }, { "coal_ore",-1 }   }, 1, 5, "coal_ore",   "furnace_animation", Pos{ 0,32 }));
	recipePrototypes.emplace_back(RecipeProto({ { "copper_ingot",1 }, { "copper_ore",-1 }, { "coal_ore",-1 }   }, 1, 5, "coal_ore",   "furnace_animation", Pos{ 0,32 }));
	recipePrototypes.emplace_back(RecipeProto({ { "clay_brick"  ,1 }, { "clay",-1 },       { "coal_ore",-1 }   }, 1, 5, "coal_ore",   "furnace_animation", Pos{ 0,32 }));
	recipePrototypes.emplace_back(RecipeProto({ { "copper_ingot",1 }, { "iron_ingot",-1 }, { "iron_ingot",-1 } }, 1, 5, "iron_ingot", "furnace_animation", Pos{ 0,32 }));

	for (const RecipeProto& recipeProto : recipePrototypes)
	{
		CraftingClass newRecipe;
		newRecipe.recipeIndex = uint16_t(program.craftingRecipes.size());

		// Recipe Population
		std::vector<RecipeComponent> recipe;
		for (const RecipeCompProto& recipeCompProto : recipeProto.recipe)
		{
			uint16_t itemIndex = findInVector(itemPrototypes, recipeCompProto.itemName).second;
			if (itemIndex == -1)
			{
				OutputDebugStringA(("Recipe Load Fail. Item: " + recipeCompProto.itemName + " does not exist. ").c_str());
			}
			else
			recipe.emplace_back(RecipeComponent{ itemIndex, recipeCompProto.requirement, recipeCompProto.resultState });
		}
		newRecipe.recipe = recipe;
		newRecipe.width = recipeProto.recipeWidth;
		newRecipe.craftTicks = recipeProto.craftingTime * 60;
		uint16_t catalystIndex = findInVector(itemPrototypes, recipeProto.catalyst).second;
		
		// Animation Population
		newRecipe.animationTextureRef = findInVector(animationPrototypes, recipeProto.animation).second;
		// Animation slides is equal to total texture width / 32 * recipe width. So that animation slides are tile aligned
		newRecipe.animationJump = newRecipe.width * 32;
		newRecipe.animationOffset = recipeProto.animationOffset;
		newRecipe.animationFrames = program.animationTextures[newRecipe.animationTextureRef].getSize().x / newRecipe.animationJump;
		// Add 1 to animationTextureRef since 0 is no animation
		++newRecipe.animationTextureRef;

		program.itemRecipeList[catalystIndex].emplace_back(newRecipe.recipeIndex);
		program.craftingRecipes.emplace_back(newRecipe);
	}
}

void LoadLogicToHotbar()
{
	Redirector* redirectAnything = new Redirector(anything,false);
	Redirector* redirectNothing  = new Redirector(nothing, false);
	Redirector* redirectDrop	 = new Redirector(anything, true);
	Holder* holder = new Holder();
	PressurePlate* pressureplate = new PressurePlate();
	Wire* wire = new Wire();
	Inverter* inverter = new Inverter();
	Booster* booster = new Booster();
	
	program.hotbar.emplace_back(nullptr);
	program.hotbar.emplace_back(redirectAnything);
	program.hotbar.emplace_back(redirectNothing);
	program.hotbar.emplace_back(redirectDrop);
	program.hotbar.emplace_back(holder);
	program.hotbar.emplace_back(pressureplate);
	program.hotbar.emplace_back(wire);
	program.hotbar.emplace_back(inverter);
	program.hotbar.emplace_back(booster);
}