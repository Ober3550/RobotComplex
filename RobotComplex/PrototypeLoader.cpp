#include <regex>
#include <cctype>
#include <SFML/Graphics.hpp>
#include "WorldSave.h"
#include "ProgramData.h"
#include "Pos.h"
#include "LogicTile.h"
#include <string>
#include "MyMod.h"
#include "CraftingProcess.h"
#include "RedirectorColors.h"
#include "TestWorld.h"
#include "PrototypeLoader.h"
#include "RecipePrototype.h"
#include "Textures.h"
#include "FindInVector.h"
#include "Animation.h"
#include "GetFileNamesInFolder.h"


sf::Texture* LoadTexture(std::string filename)
{
	sf::Texture* texture = new sf::Texture();
	//sf::Texture* texture = nullptr;
	if (!texture->loadFromFile("Assets/x32/" + filename))
	{
		OutputDebugStringA((filename + " failed to load\r\n").c_str());
	}
	return texture;
}

void LoadAllTextures()
{
	std::string fontIndex = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ.";
	for (int i = 0; i < (int)fontIndex.length(); i++)
	{
		sf::IntRect fontRect = sf::IntRect(i * 3, 0, 3, 5);
		program.fontMap.insert({ fontIndex[i], fontRect });
	}
}



void LoadPrototypes()
{
	LoadAllTextures();

	// Ground Tiles
	std::string groundFolder = "new_ground";
	std::vector<std::string> groundPrototypes = getFileNamesInFolder("Assets/x32/" + groundFolder);
	for (const std::string& text : groundPrototypes)
	{
		groundTextures.emplace_back(LoadTexture(groundFolder +"/" + text));
	}

	// Items
	program.itemPrototypes = {
		"",
		"anything",
		"energy",
		"coal_ore",
		"iron_ore",
		"copper_ore",
		"clay",
		"iron_ingot",
		"copper_ingot",
		"clay_brick",
		"steel_ingot",
		"copper_plate",
		"copper_wire",
		"iron_plate",
		"iron_gear",
		"iron_rod"
	};
	program.regItemsEnd = program.itemPrototypes.size();
	// Add logical elements to the end of the list to allow for crafting them
	for(std::string logic : logicTypes)
		program.itemPrototypes.emplace_back(logic);
	std::regex addSpaces("_");
	// Populate the item tooltips according to the prototype names
	for (std::string text : program.itemPrototypes)
	{
		std::string temp = "";
		temp = std::regex_replace(text, addSpaces, " ");
		uint16_t i = 0;
		temp[i] = std::toupper(temp[i]);
		for (i = 1; i < temp.length(); i++)
		{
			if (temp[i - 1] == ' ')
				temp[i] = std::toupper(temp[i]);
		}
		program.itemTooltips.emplace_back(temp);
	}
	for (int i=0;i<program.regItemsEnd;i++)
	{
		itemTextures.emplace_back(LoadTexture("items/" + program.itemPrototypes[i] + ".png"));
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
	program.logicTooltips.emplace_back(logicTypeTooltips);	// Comparer
	program.logicTooltips[0].emplace_back("Wire transfers signals");
	program.logicTooltips[1].emplace_back("Does nothing when powered");
	program.logicTooltips[1].emplace_back("Redirects the robot");
	program.logicTooltips[2].emplace_back("Creates signal when robot or item ontop");
	program.logicTooltips[3].emplace_back("Inverter: output = 16 - behind + side");
	program.logicTooltips[4].emplace_back("Booster: output = 16 + behind if: behind > side");
	program.logicTooltips[5].emplace_back("Gives out same signal as input");
	program.logicTooltips[6].emplace_back("Stores 16 different values that cycle when input rises to high");
	program.logicTooltips[7].emplace_back("Catches robots and releases when powered");
	program.logicTooltips[8].emplace_back("Counts up to the max value then resets");
	program.logicTooltips[9].emplace_back("Comparer: output = 16 + input if: behind = side");

	// Animation prototypes
	std::vector<std::string> animationPrototypes;
	animationPrototypes.emplace_back("furnace_animation");
	Animation newAnimation;
	newAnimation.animationSpeed = 10;
	newAnimation.animationType = ping;
	newAnimation.frameWidth = 32;
	newAnimation.animationOffset = Pos{ 0,32 };
	program.animationTemplates.emplace_back(newAnimation);

	for (const std::string& text : animationPrototypes)
	{
		animationTextures.emplace_back(LoadTexture("machines/" + text + ".png"));
	}

	// Recipe Prototypes: Recipe, Recipe Width, Craft Time, Recipe Catalyst, Animation, Animation Offset
	std::vector<RecipeProto> recipePrototypes;
	recipePrototypes.emplace_back(RecipeProto({ { "wire"  ,1 }, { "iron_ore",  -1 }, { "coal_ore",-1 }   }, 1, 5, "coal_ore",   "furnace_animation"));
	recipePrototypes.emplace_back(RecipeProto({ { "copper_ingot",1 }, { "copper_ore",-1 }, { "coal_ore",-1 }   }, 1, 5, "coal_ore",   "furnace_animation"));
	recipePrototypes.emplace_back(RecipeProto({ { "clay_brick"  ,1 }, { "clay",-1 },       { "coal_ore",-1 }   }, 1, 5, "coal_ore",   "furnace_animation"));
	recipePrototypes.emplace_back(RecipeProto({ 
		{ "steel_ingot",1 }, { "steel_ingot",1 }, 
		{ "iron_ingot",-1 }, { "iron_ingot",-1 }, 
		{ "iron_ingot",-1 }, { "coal_ore",-1 } 
		}, 2, 5, "coal_ore", ""));

	// Tier 2 Products
	// Gear Shape
	recipePrototypes.emplace_back(RecipeProto({
		{ "",0 }, { "iron_ingot",-1 }, { "",0 },
		{ "iron_ingot",-1 }, { "iron_gear",1 }, { "iron_ingot",-1 },
		{ "",0 }, { "iron_ingot",-1 }, { "",0 },
		}, 3, 1, "iron_ingot", ""));

	recipePrototypes.emplace_back(RecipeProto({
		{ "",0 }, { "copper_ingot",-1 }, { "",0 },
		{ "copper_ingot",-1 }, { "copper_wire",1 }, { "copper_ingot",-1 },
		{ "",0 }, { "copper_ingot",-1 }, { "",0 },
		}, 3, 1, "copper_ingot", ""));

	// Plates
	recipePrototypes.emplace_back(RecipeProto({
		{ "",0 }, { "iron_plate",1 }, 
		{ "iron_ingot",-1 }, { "iron_ingot",-1 },
		{ "iron_ingot",-1 }, { "iron_ingot",-1 },
		}, 2, 1, "iron_ingot", ""));

	recipePrototypes.emplace_back(RecipeProto({
		{ "",0 }, { "copper_plate",1 },
		{ "copper_ingot",-1 }, { "copper_ingot",-1 },
		{ "copper_ingot",-1 }, { "copper_ingot",-1 },
		}, 2, 1, "copper_ingot", ""));

	/*
	recipePrototypes.emplace_back(RecipeProto({
		{ "",0 }, { "clay",-1 }, { "clay",-1 },{ "clay",-1 }, { "",0 },
		{ "clay",-1 }, { "clay",-1 }, { "",0 },{ "clay",-1 }, { "clay",-1 },
		{ "clay",-1 }, { "",0 }, { "die_gear",1 },{ "",0 }, { "clay",-1 },
		{ "clay",-1 }, { "clay",-1 }, { "",0 },{ "clay",-1 }, { "clay",-1 },
		{ "",0 }, { "clay",-1 }, { "clay",-1 },{ "clay",-1 }, { "",0 }
		}, 5, 1, "clay", "", Pos{ 0,0 }));
		*/	

	for (const RecipeProto& recipeProto : recipePrototypes)
	{
		CraftingClass newRecipe;
		newRecipe.recipeIndex = uint16_t(program.craftingRecipes.size());

		// Recipe Population
		std::vector<RecipeComponent> recipe;
		for (const RecipeCompProto& recipeCompProto : recipeProto.recipe)
		{
			std::pair<bool,int> itemIndex = findInVector(program.itemPrototypes, recipeCompProto.itemName);
			if (!itemIndex.first)
			{
				OutputDebugStringA(("Recipe Load Fail. Item: " + recipeCompProto.itemName + " does not exist. ").c_str());
				assert(false);
			}
			else
			recipe.emplace_back(RecipeComponent{ (uint16_t)itemIndex.second, recipeCompProto.requirement, recipeCompProto.resultState });
		}
		newRecipe.recipe = recipe;
		newRecipe.width = recipeProto.recipeWidth;
		newRecipe.height = (uint8_t)(float(recipe.size()) / newRecipe.width);
		newRecipe.craftTicks = recipeProto.craftingTime * GC::UPDATERATE;
		uint16_t catalystIndex = findInVector(program.itemPrototypes, recipeProto.catalyst).second;
		newRecipe.animationReference = (findInVector(animationPrototypes, recipeProto.animation).second + 1);

		program.itemRecipeList[catalystIndex].emplace_back(newRecipe.recipeIndex);
		program.craftingRecipes.emplace_back(newRecipe);
	}
}

void LoadLogicToHotbar()
{
	Redirector* redirect = new Redirector();
	Gate* gate = new Gate();
	PressurePlate* pressureplate = new PressurePlate();
	Wire* wire = new Wire();
	Inverter* inverter = new Inverter();
	Booster* booster = new Booster();
	Belt* belt = new Belt();
	WireBridge* wireBridge = new WireBridge();
	Comparer* comparer = new Comparer();
	Robot* robot = new Robot();
	ItemTile* item = new ItemTile();
	item->itemTile = 4;
	item->quantity = 1;
	
	program.hotbar.emplace_back(nullptr);
	program.hotbar.emplace_back(redirect);
	program.hotbar.emplace_back(belt);
	program.hotbar.emplace_back(pressureplate);
	program.hotbar.emplace_back(gate);
	program.hotbar.emplace_back(wire);
	program.hotbar.emplace_back(wireBridge);
	program.hotbar.emplace_back(inverter);
	program.hotbar.emplace_back(booster);
	program.hotbar.emplace_back(comparer);
	program.hotbar.emplace_back(robot);
	program.hotbar.emplace_back(item);
	program.hotbar.emplace_back(nullptr);
	program.hotbar.emplace_back(nullptr);
	program.hotbar.emplace_back(nullptr);
	program.hotbar.emplace_back(nullptr);
	program.hotbar.emplace_back(nullptr);
	program.hotbar.emplace_back(nullptr);
	program.hotbar.emplace_back(nullptr);
	program.hotbar.emplace_back(nullptr);
}