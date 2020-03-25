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

extern "C"
{
#include "Lua/include/lua.h";
#include "Lua/include/lauxlib.h";
#include "Lua/include/lualib.h";
}

#ifdef _WIN32
#pragma comment(lib, "lua53.lib")
#endif

sf::Texture* LoadTexture(std::string filename)
{
	sf::Texture* texture = new sf::Texture();
	//sf::Texture* texture = nullptr;
	if (!texture->loadFromFile("assets/" + filename))
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

bool CheckLua(lua_State* L, int r)
{
	if (r != LUA_OK)
	{
		std::string errormsg = lua_tostring(L, -1);
		std::cout << errormsg << std::endl;
		return false;
	}
	return true;
}

static void lua_append_linear_table(lua_State* L, int index, std::vector<std::string>* vec)
{
	// Push another reference to the table on top of the stack (so we know
	// where it is, and this function can work for negative, positive and
	// pseudo indices
	lua_pushvalue(L, index);
	// stack now contains: -1 => table
	lua_pushnil(L);
	// stack now contains: -1 => nil; -2 => table
	while (lua_next(L, -2))
	{
		// stack now contains: -1 => value; -2 => key; -3 => table
		// copy the key so that lua_tostring does not modify the original
		lua_pushvalue(L, -2);
		// stack now contains: -1 => key; -2 => value; -3 => key; -4 => table
		//const char* key = lua_tostring(L, -1);
		const char* value = lua_tostring(L, -2);
		vec->emplace_back(value);
		//printf("%s => %s\n", key, value);
		// pop value + copy of key, leaving original key
		lua_pop(L, 2);
		// stack now contains: -1 => key; -2 => table
	}
	// stack now contains: -1 => table (when lua_next returns 0 it pops the key
	// but does not push anything.)
	// Pop table
	lua_pop(L, 1);
	// Stack is now the same as it was on entry to this function
}

void LoadPrototypes()
{
	LoadAllTextures();

	// Ground Tiles
	groundTexture = LoadTexture("groundTexture8greyscale.png");

	lua_State* L = luaL_newstate();
	// Items
	if (CheckLua(L, luaL_dofile(L, "data/items.lua")))
	{
		lua_getglobal(L, "items");
		if (lua_istable(L, -1))
		{
			lua_append_linear_table(L, -1, &program.itemPrototypes);
		}
	}

	program.regItemsEnd = program.itemPrototypes.size();
	// Add logical elements to the end of the list to allow for crafting them
	for (std::string logic : logicTypes)
		program.itemPrototypes.emplace_back(logic);
	program.itemPrototypes.emplace_back("robot");


	// Populate the item tooltips according to the prototype names
	std::regex addSpaces("_");
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
	itemTextures.emplace_back(&sf::Texture());
	itemTextures.emplace_back(&sf::Texture());
	itemTextures.emplace_back(&sf::Texture());
	for (int i = 3; i < program.regItemsEnd; i++)
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
		animationTextures.emplace_back(LoadTexture("animations/" + text + ".png"));
	}

	// Recipes loaded from lua
	if (CheckLua(L, luaL_dofile(L, "data/recipes.lua")))
	{
		lua_getglobal(L, "recipes");
		if (lua_istable(L, -1))
		{
			lua_pushnil(L);
			// stack now contains: -1 => nil; -2 => table
			while (lua_next(L, -2))
			{
				std::string recipeName = "";
				RecipeProto newRecipe;
				lua_pushvalue(L, -2);
				if (lua_isstring(L, -1))
				{
					recipeName = lua_tostring(L, -1);
				}
				if (lua_istable(L, -2))
				{
					lua_pushstring(L, "width");
					lua_gettable(L, -3);
					newRecipe.recipeWidth = lua_tonumber(L, -1);
					lua_pop(L, 1);

					lua_pushstring(L, "craft_time");
					lua_gettable(L, -3);
					newRecipe.craftingTime = lua_tonumber(L, -1);
					lua_pop(L, 1);

					lua_pushstring(L, "animation");
					lua_gettable(L, -3);
					newRecipe.animation = lua_tostring(L, -1);
					lua_pop(L, 1);

					lua_pushstring(L, "components");
					lua_gettable(L, -3);
					if (lua_istable(L,-1))
					{
						lua_pushnil(L);
						while (lua_next(L, -2))
						{
							RecipeCompProto newComponent;
							// For Components
							lua_pushvalue(L, -2);
							int index;
							if(lua_isnumber(L,-1))
								index = lua_tonumber(L, -1);
							if (lua_istable(L, -2))
							{
								lua_rawgeti(L, -2, 1);
								if(lua_isstring(L,-1))	
									newComponent.itemName = lua_tostring(L, -1);
								lua_pop(L, 1);

								lua_rawgeti(L, -2, 2);
								if (lua_isnumber(L, -1))
									newComponent.resultState = lua_tonumber(L, -1);
								lua_pop(L, 1);

								lua_rawgeti(L, -2, 3);
								if (lua_isnumber(L, -1))
									newComponent.requirement = lua_tonumber(L, -1);
								lua_pop(L, 1);
							}
							lua_pop(L, 2);
							newRecipe.recipe.emplace_back(newComponent);
						}
					}
					lua_pop(L, 1);
				}
				lua_pop(L, 2);
				program.recipePrototypes.insert({ recipeName,newRecipe });
			}
			lua_pop(L, 1);

		}
	}

	// Recipe Prototypes: Recipe, Recipe Width, Craft Time, Animation, Animation Offset
	/*
	program.recipePrototypes.insert({"iron_ingot",RecipeProto({ { "iron_ingot"  ,1 }, { "iron_ore",  -1 }, { "coal_ore",-1 } }, 1, 5, "furnace_animation")});

	*/
	MySet<uint16_t> catalysts = MySet<uint16_t>();
	for (std::pair<std::string, RecipeProto> recipeProto : program.recipePrototypes)
	{
		catalysts.clear();
		CraftingClass newRecipe;
		newRecipe.recipeIndex = uint16_t(program.craftingRecipes.size());

		// Recipe Population
		std::vector<RecipeComponent> recipe;
		for (RecipeCompProto recipeCompProto : recipeProto.second.recipe)
		{
			std::pair<bool,int> itemIndex = findInVector(program.itemPrototypes, recipeCompProto.itemName);
			if (!itemIndex.first)
			{
				OutputDebugStringA(("Recipe Load Fail. Item: " + recipeCompProto.itemName + " does not exist. ").c_str());
				assert(false);
			}
			else
			{
				if (itemIndex.second)
				{
					catalysts.insert(itemIndex.second);
					recipe.emplace_back(RecipeComponent{ (uint16_t)itemIndex.second, recipeCompProto.requirement, recipeCompProto.resultState });
				}
				else
				{
					recipe.emplace_back(RecipeComponent{ 0,0,0 });
				}
			}
		}
		newRecipe.recipe = recipe;
		newRecipe.width = recipeProto.second.recipeWidth;
		newRecipe.height = (uint8_t)(float(recipe.size()) / newRecipe.width);
		newRecipe.craftTicks = recipeProto.second.craftingTime * GC::UPDATERATE;
		newRecipe.animationReference = (findInVector(animationPrototypes, recipeProto.second.animation).second + 1);
		program.craftingRecipes.emplace_back(newRecipe);

		for (uint16_t catalystIndex : catalysts)
		{
			program.itemRecipeList[catalystIndex].emplace_back(newRecipe.recipeIndex);
		}
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
	Robot* robot2 = new Robot();
	ItemTile* item = new ItemTile(4);
	ItemTile* item2 = new ItemTile(3);
	PlusOne* plusone = new PlusOne();
	Shover* shover = new Shover();
	Toggle* toggle = new Toggle();

	int godQuantity = 200;
	redirect->quantity = godQuantity;
	gate->quantity = godQuantity;
	pressureplate->quantity = godQuantity;
	wire->quantity = godQuantity;
	inverter->quantity = godQuantity;
	booster->quantity = godQuantity;
	belt->quantity = godQuantity;
	wireBridge->quantity = godQuantity;
	comparer->quantity = godQuantity;
	item->quantity = godQuantity;
	item2->quantity = godQuantity;
	plusone->quantity = godQuantity;
	shover->quantity = godQuantity;
	toggle->quantity = godQuantity;

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
	program.hotbar.emplace_back(robot2);
	program.hotbar.emplace_back(item);
	program.hotbar.emplace_back(item2);
	program.hotbar.emplace_back(plusone);
	program.hotbar.emplace_back(shover);
	program.hotbar.emplace_back(toggle);
}