#pragma once
#include "Pos.h"
#include <array>
#include "Tile.h"
#include "Constants.h"
#include "MyMap.h"
#include "ItemTile.h"
#include "Robot.h"
#include "LogicTile.h"

//struct PosItem {
//	Pos pos;
//	ItemTile second;
//};
//
//struct PosLogic {
//	Pos pos;
//	LogicTile second;
//};
//
//struct PosRobot {
//	Pos pos;
//	Robot second;
//};

class WorldChunk {
public:
	Pos chunkPos = Pos{ 0,0 };
	std::array<GroundTile, GC::chunkTileNum> tiles;
	//std::vector<PosItem> items;
	//std::vector<PosLogic> logic;
	//std::vector<PosRobot> robots;
	GroundTile* GetTile(Pos worldPos)
	{
		int index = (worldPos.y << GC::chunkShift) | worldPos.x;
		return &tiles[index];
	}
	ItemTile* GetItem(Pos worldPos);
	LogicTile* GetLogic(Pos worldPos);
	Robot* GetRobot(Pos worldPos);
};
