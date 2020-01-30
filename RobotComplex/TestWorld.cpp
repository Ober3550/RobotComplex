
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
	
}

void CreateTestWorld2()
{
	world.clear();
	world.GenerateChunk(Pos{ 0,0 });
	world.GenerateChunk(Pos{ -1,0 });
	Robot robot;
	robot.stopped = true;
	robot.facing = north;
	robot.pos = { -3,7 };
	world.robots.insert({ robot.pos.CoordToEncoded(), robot });

	Robot robot2;
	robot2.stopped = true;
	robot2.facing = east;
	robot2.pos = { -5,5 };
	world.robots.insert({ robot2.pos.CoordToEncoded(), robot2 });

	ItemTile* tile = &world.items[Pos{ -3, 6 }.CoordToEncoded()];
	tile->itemTile = 4;
	tile->quantity = 1;

	tile = &world.items[Pos{ -4, 5 }.CoordToEncoded()];
	tile->itemTile = 4;
	tile->quantity = 1;

	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			world.platforms[Pos{ -5 + i, - 5 + j }.CoordToEncoded()] = 10;
		}
	}
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