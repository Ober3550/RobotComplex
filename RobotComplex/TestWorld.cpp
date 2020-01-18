
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
#include "TestWorld.h"
#include "FindInVector.h"

void CreateTestWorld()
{
	world.worldChunks.clear();
	world.GenerateChunk(Pos{ 0,0 });
	world.GenerateChunk(Pos{ -1,0 });
	Robot robot;
	robot.facing = north;
	robot.pos = { -3,17 };
	world.robots.insert({ robot.pos.CoordToEncoded(), robot });
	for (int y = 1; y < 17; y++)
	{
		for (int x = 0; x < 18; x++)
		{
			switch (x)
			{
			case 17: {
				LogicTile* tile;
				tile = new Wire();
				tile->pos = { x,y };
				world.logictiles.insert({ tile->pos.CoordToEncoded(),tile });
			}break;
			case 16: {
				LogicTile* tile;
				tile = new Booster();
				tile->pos = { x,y };
				tile->facing = west;
				world.logictiles.insert({ tile->pos.CoordToEncoded(),tile });
			}break;
			default: {
				Redirector* tile;
				tile = new Redirector();
				tile->facing = south;
				tile->pos = { x,y };
				tile->colorClass = MyMod(y, 2) == 0 ? red : cyan;
				tile->itemFilter = anything;
				tile->dropItem = true;
				world.logictiles.insert({ tile->pos.CoordToEncoded(),tile });
			}break;
			}
		}
	}
	for (int x = 0; x < 16; x++)
	{
		Redirector* tile;
		tile = new Redirector();
		tile->pos = { x,17 };
		tile->colorClass = MyMod(17, 2) == 0 ? red : cyan;
		tile->itemFilter = anything;
		world.logictiles.insert({ tile->pos.CoordToEncoded(),tile });

		tile = new Redirector();
		tile->pos = { x,20 };
		tile->itemFilter = nothing;
		tile->facing = west;
		world.logictiles.insert({ tile->pos.CoordToEncoded(),tile });
	}
	LogicTile* memoryX;
	memoryX = new Counter();
	memoryX->pos = { -1, 17 };
	memoryX->facing = east;
	memoryX->signal = 16;
	world.logictiles.insert({ memoryX->pos.CoordToEncoded(),memoryX });

	LogicTile* plate;
	plate = new PressurePlate();
	plate->pos = { -2,17 };
	world.logictiles.insert({ plate->pos.CoordToEncoded(),plate });

	LogicTile* memoryY;
	memoryY = new Counter();
	memoryY->pos = { 17, 19 };
	memoryY->facing = north;
	memoryY->signal = 0;
	world.logictiles.insert({ memoryY->pos.CoordToEncoded(),memoryY });

	LogicTile* plate2;
	plate2 = new PressurePlate();
	plate2->pos = { 17,20 };
	world.logictiles.insert({ plate2->pos.CoordToEncoded(),plate2 });
	// Wire to connect mem2 to plate 2
	LogicTile* wire;
	wire = new Wire();
	wire->pos = { 17,17 };
	world.logictiles.insert({ wire->pos.CoordToEncoded(),wire });

	LogicTile* inv = new Inverter();
	inv->pos = { 17,18 };
	inv->facing = north;
	world.logictiles.insert({ inv->pos.CoordToEncoded(),inv });
	world.updateQueueC.insert({ inv->pos.CoordToEncoded() });

	// East y direct
	Redirector* redirector;
	redirector = new Redirector();
	redirector->pos = { 15,19 };
	redirector->facing = east;
	redirector->itemFilter = nothing;
	world.logictiles.insert({ redirector->pos.CoordToEncoded(),redirector });

	// South y direct
	redirector = new Redirector();
	redirector->pos = { 18,19 };
	redirector->facing = south;
	redirector->itemFilter = nothing;
	world.logictiles.insert({ redirector->pos.CoordToEncoded(),redirector });

	// West y direct
	redirector = new Redirector();
	redirector->pos = { 18,20 };
	redirector->facing = west;
	redirector->itemFilter = nothing;
	world.logictiles.insert({ redirector->pos.CoordToEncoded(),redirector });

	//Redirector to send to start
	redirector = new Redirector();
	redirector->pos = { -3,20 };
	redirector->facing = north;
	redirector->itemFilter = nothing;
	world.logictiles.insert({ redirector->pos.CoordToEncoded(),redirector });
	//Redirector for input
	redirector = new Redirector();
	redirector->pos = { -3,17 };
	redirector->facing = east;
	redirector->itemFilter = anything;
	world.logictiles.insert({ redirector->pos.CoordToEncoded(),redirector });
	// Coal on ground
	ItemTile* tile = &world.items[Pos{ -3, 15 }.CoordToEncoded()];
	tile->itemTile = 3;
	tile->quantity = 255;
	// Iron on ground
	tile = &world.items[Pos{ -5, 17 }.CoordToEncoded()];
	tile->itemTile = 4;
	tile->quantity = 255;
	// Copper on ground
	tile = &world.items[Pos{ -6, 17 }.CoordToEncoded()];
	tile->itemTile = 5;
	tile->quantity = 255;
	// Clay on ground
	tile = &world.items[Pos{ -7, 17 }.CoordToEncoded()];
	tile->itemTile = 6;
	tile->quantity = 255;

	/*
	// Create crafting class
	RecipeComponent coal = { 3, 1, -1 };	// Coal
	RecipeComponent iron_ore = { 4, 1, -1 };	// Iron Ore
	RecipeComponent copper_ore = { 5, 1, -1 };	// Copper Ore
	RecipeComponent iron_ingot = { 6, 1,  1 };	// Iron Ingot
	RecipeComponent iron_ingot_consume = { 6, 1,  -1 };	// Iron Ingot
	RecipeComponent copper_ingot = { 7, 1,  1 }; // Copper ingot
	RecipeComponent clay = { 8, 1, -1 }; // Clay
	RecipeComponent clay_brick = { 9, 1,  1 }; // Cay Brick

	program.itemRecipeList.insert({ 3,std::vector<uint16_t>() });
	program.craftingRecipes.emplace_back(CraftingClass(0, { iron_ingot,iron_ore,coal }, 1, 5, 1, 4, Pos{0,32}));
	program.craftingRecipes.emplace_back(CraftingClass(1, { copper_ingot,copper_ore,coal }, 1, 5, 1, 4, Pos{ 0,32 }));
	program.craftingRecipes.emplace_back(CraftingClass(2, { clay_brick,clay,coal }, 1, 5, 0, 0, Pos{ 0,0 }));
	program.craftingRecipes.emplace_back(CraftingClass(3, { copper_ingot,iron_ingot_consume,iron_ingot_consume}, 1, 2, 1, 4, Pos{ 0,32 }));

	program.itemRecipeList[3].emplace_back(0);
	program.itemRecipeList[3].emplace_back(1);
	program.itemRecipeList[3].emplace_back(2);
	program.itemRecipeList[6].emplace_back(3);
	*/
}

void CreateTestWorld2()
{
	world.clear();
	world.GenerateChunk(Pos{ 0,0 });
	world.GenerateChunk(Pos{ -1,0 });
	Robot robot;
	robot.facing = north;
	robot.pos = { -3,17 };
	world.robots.insert({ robot.pos.CoordToEncoded(), robot });

	std::pair<bool, int> getItemNumber;
	//std::vector<std::string> items = { "clay", , "copper_ore",   };

	std::string itemName = "iron_ore";
	getItemNumber = findInVector(program.itemPrototypes, itemName);
	for (int i = 0; i < 5; i++)
	{
		if (getItemNumber.first)
		{
			ItemTile* tile = &world.items[Pos{ -10 - i, 10 }.CoordToEncoded()];
			tile->itemTile = getItemNumber.second;
			tile->quantity = 255;
		}
		else
		{
			OutputDebugStringA(("Failed to place " + itemName + "\r\n").c_str());
		}
	}
	for (int i = 0; i < 5; i++)
	{
		std::string itemName = "coal_ore";
		getItemNumber = findInVector(program.itemPrototypes, itemName);
		if (getItemNumber.first)
		{
			ItemTile* tile = &world.items[Pos{ 10 + i, 11 }.CoordToEncoded()];
			tile->itemTile = getItemNumber.second;
			tile->quantity = 255;
		}
		else
		{
			OutputDebugStringA(("Failed to place " + itemName + "\r\n").c_str());
		}
	}
}