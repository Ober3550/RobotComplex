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
	MyMap<uint64_t, uint16_t> platforms;
	MySet<uint64_t> connectedPlatform;
	MyMap<uint64_t, Facing> nextPlatforms;
	MySet<uint64_t> platformMovingTo;

	MyMap<uint64_t, ItemTile> items;
	MyMap<uint64_t, Facing> nextItemPos;
	MySet<uint64_t> prevItemMovingTo;
	MySet<uint64_t> itemMovingTo;
	MySet<uint64_t> itemPrevMoved;

	MyMap<uint64_t, Robot> robots;
	MyMap<uint64_t, Facing> nextRobotPos;
	MySet<uint64_t> robotMovingTo;

	MyMap<uint64_t, LogicTile*> logictiles;

	MyMap<uint64_t, WorldChunk> worldChunks;
	MyMap<uint64_t, CraftingProcess> craftingQueue;
	MySet<uint64_t> updateQueueA;	// Processing queue for update from Queue B
	MySet<uint64_t> updateQueueB;	// Queue for current updates
	MyMap<uint64_t,int> updateQueueC;	// Queue for updates next ticks
	MySet<uint64_t> updateQueueD;	// Queue for item updates next tick
	bool moving = false;
	std::string name;
	uint64_t tick;
	int seed = 0;
	// Variables for keeping elements and data structure organised between versions/changes
	MyMap<uint16_t, uint16_t> oldItemNewItem;	// A map from the old items to new items
	MyMap<uint8_t, uint8_t> oldLogicNewLogic;
	std::vector<uint16_t> oldLogicSize;

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
		noiseRef->SetFractalOctaves(6);
		noiseRef->SetFractalLacunarity(2);
		noiseRef->SetFractalGain(0.5f);
		noiseRef->SetAxisScales(0.4f,0.4f,0.4f);
		noiseRef->SetNoiseType(FastNoiseSIMD::NoiseType::ValueFractal);
		this->name = "New World";
		this->tick = 0;
	}
	WorldSave(int seed)
	{
		this->seed = seed;
		noiseRef = FastNoiseSIMD::NewFastNoiseSIMD();
		noiseRef->SetFractalOctaves(6);
		noiseRef->SetFractalLacunarity(2);
		noiseRef->SetFractalGain(0.5f);
		noiseRef->SetAxisScales(0.4f, 0.4f, 0.4f);
		noiseRef->SetNoiseType(FastNoiseSIMD::NoiseType::ValueFractal);
		noiseRef->SetSeed(seed);
		this->name = "New World";
		this->tick = 0;
	}
	bool CheckMovePlatform(Pos pos, Facing toward);
	void MovePlatform(Pos pos, Facing toward);
	bool PushItems(std::vector<Pos>* itemsMoving, Facing toward, int pushesLeft); // bool is for whether the robot should move after a failed operation
	bool ChangeItem(Pos pos, uint16_t item, int quantity);
	bool ChangeRobot(Pos pos, int quantity);
	bool ChangeLogic(Pos pos, uint8_t logicType, int quantity);
	void GenerateChunk(Pos pos);
	void GenerateOre(Pos pos);
	void Serialize(std::string filename);
	void SerializeItemNames(std::string filename);
	void SerializeMisc(std::string filename);
	void SerializeLogicStructure(std::string filename);
	void Deserialize(std::string filename);
	void DeserializeItemNames(std::string filename);
	void DeserializeLogicStructure(std::string filename);
	void DeserializeMisc(std::string filename);
	void clear();
};
extern WorldSave world;
