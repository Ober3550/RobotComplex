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

	program.itemsEnd = program.itemPrototypes.size() - 1;
	// Add logical elements to the end of the list to allow for crafting them
	for (auto logic : logicTypes)
	{
		logicTextures.emplace(logic.first, LoadTexture("logic/"+logic.second+".png"));
		program.itemPrototypes.emplace_back(logic.second);
	}
		
	program.itemPrototypes.emplace_back("robot");


	// Populate the item tooltips according to the prototype names
	std::regex addSpaces("_");
	for (int i=0;i < program.itemPrototypes.size();i++)
	{
		std::string text = program.itemPrototypes[i];
		std::string temp = "";
		temp = std::regex_replace(text, addSpaces, " ");
		uint16_t j = 0;
		temp[j] = std::toupper(temp[j]);
		for (j = 1; j < temp.length(); j++)
		{
			if (temp[j - 1] == ' ')
				temp[j] = std::toupper(temp[j]);
		}
		program.itemTooltips.insert({ i,temp });
	}
	itemTextures.emplace_back(&sf::Texture());
	itemTextures.emplace_back(&sf::Texture());
	itemTextures.emplace_back(&sf::Texture());
	for (int i = 3; i <= program.itemsEnd; i++)
	{
		itemTextures.emplace_back(LoadTexture("items/" + program.itemPrototypes[i] + ".png"));
	}

	// Logic Tooltips
	program.itemTooltips.insert({ program.itemsEnd + wire, "Wire: transfers signals" });
	program.itemTooltips.insert({ program.itemsEnd + redirector, "Redirector: Redirects the robot" });
	program.itemTooltips.insert({ program.itemsEnd + pressureplate, "PressurePlate: Creates signal when robot or item ontop" });
	program.itemTooltips.insert({ program.itemsEnd + inverter, "Inverter: output = 16 - behind + side" });
	program.itemTooltips.insert({ program.itemsEnd + booster, "Booster: output = 16 + behind if: behind > side" });
	program.itemTooltips.insert({ program.itemsEnd + repeater, "Repeater: Gives out same signal as input" });
	program.itemTooltips.insert({ program.itemsEnd + gate, "Gate: Can stop robots movement" });
	program.itemTooltips.insert({ program.itemsEnd + counter, "Counter: Counts up" });
	program.itemTooltips.insert({ program.itemsEnd + comparer, "Comparer: output = 16 + input if: behind = side" });

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

/*
	// Wire Logic
	wire = 1,
	redirector,
	gate,
	belt,
	shover,
	wirebridge,

	// Unidirectional Output
	inverter = 128,
	pressureplate,
	booster,
	counter,
	repeater,
	comparer,
	plusone,
	toggle
*/
void LoadLogicToHotbar()
{
	uint8_t godQuantity = 200;
	ItemTile wire				= ItemTile(program.itemsEnd + 1,	godQuantity);
	ItemTile redirect			= ItemTile(program.itemsEnd + 2,	godQuantity);
	ItemTile gate				= ItemTile(program.itemsEnd + 3,	godQuantity);
	ItemTile belt				= ItemTile(program.itemsEnd + 4,	godQuantity);
	ItemTile shover				= ItemTile(program.itemsEnd + 5,	godQuantity);
	ItemTile wireBridge			= ItemTile(program.itemsEnd + 6,	godQuantity);
	ItemTile inverter			= ItemTile(program.itemsEnd + 128,  godQuantity);
	ItemTile pressureplate		= ItemTile(program.itemsEnd + 129,  godQuantity);
	ItemTile booster			= ItemTile(program.itemsEnd + 130,  godQuantity);
	ItemTile comparer			= ItemTile(program.itemsEnd + 133,  godQuantity);
	ItemTile plusone			= ItemTile(program.itemsEnd + 134,  godQuantity);
	ItemTile toggle				= ItemTile(program.itemsEnd + 135,  godQuantity);
	ItemTile item				= ItemTile(4,	godQuantity);
	ItemTile item2				= ItemTile(3,	godQuantity);

	program.hotbar.insert({1, wire});
	program.hotbar.insert({ 2, inverter });
}