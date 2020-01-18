#pragma once
#include <unordered_map>
#include <unordered_set>
#include "Robot.h"
#include "LogicTile.h"
#include "WorldChunk.h"
#include "Tile.h"
#include "MyMap.h"
#include "CraftingProcess.h"
#include "ItemTile.h"
#include "MySet.h"
#include "FastNoiseSIMD/FastNoiseSIMD.h"

struct WorldSave {
public:
	MyMap<uint64_t, ItemTile> items;
	MyMap<uint64_t, uint64_t> nextItemPos;
	MySet<uint64_t> itemPrevMoved;
	MyMap<uint64_t, Robot> robots;
	MyMap<uint64_t, uint64_t> nextRobotPos;
	MyMap<uint64_t, LogicTile*> logictiles;
	MyMap<uint64_t, WorldChunk> worldChunks;
	MyMap<uint64_t, CraftingProcess> craftingQueue;
	MySet<uint64_t> updateQueueA;	// Processing queue for update from Queue B
	MySet<uint64_t> updateQueueB;	// Queue for current updates
	MySet<uint64_t> updateQueueC;	// Queue for updates next tick
	uint64_t tick;
	uint64_t seed = 0;
	FastNoiseSIMD* noiseRef;
	GroundTile*		 GetGroundTile(Pos pos);
	ItemTile*		 GetItemTile(Pos pos);
	ItemTile*		 GetItemTile(uint64_t encodedPos);
	Robot*			 GetRobot(Pos pos);
	Robot*			 GetRobot(uint64_t encodedPos);
	LogicTile*		 GetLogicTile(Pos pos);
	LogicTile*		 GetLogicTile(uint64_t encodedPos);
	CraftingProcess* GetCrafting(Pos pos);
	CraftingProcess* GetCrafting(uint64_t encodedPos);
	WorldSave()
	{
		noiseRef = FastNoiseSIMD::NewFastNoiseSIMD();
	}
	bool ChangeItem(Pos pos, uint16_t item, int quantity);
	void GenerateChunk(Pos pos);
	void Serialize(std::string filename);
	void Deserialize(std::string filename);
	void clear()
	{
		items.clear();
		robots.clear();
		logictiles.clear();
		worldChunks.clear();
		craftingQueue.clear();
		updateQueueA.clear();
		updateQueueB.clear();
		updateQueueC.clear();
		tick = 0;
	}
};
extern WorldSave world;
