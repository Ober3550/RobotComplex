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

	MyMap<uint64_t, LogicTile> logicTiles;

	MyMap<uint64_t, WorldChunk> worldChunks;
	MyMap<uint64_t, CraftingProcess> craftingQueue;
	MySet<uint64_t> updateCurr;	// Processing queue for update from Queue B
	MySet<uint64_t> updateProp;	// Queue for current updates
	MyMap<uint64_t,int> updateNext;	// Queue for updates next ticks
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
		this->name = "";
		this->tick = 0;
	}
	WorldSave(std::string name)
	{
		this->seed = int(std::hash<std::string>{}(name));
		noiseRef = FastNoiseSIMD::NewFastNoiseSIMD();
		noiseRef->SetFractalOctaves(6);
		noiseRef->SetFractalLacunarity(2);
		noiseRef->SetFractalGain(0.5f);
		noiseRef->SetAxisScales(0.4f, 0.4f, 0.4f);
		noiseRef->SetNoiseType(FastNoiseSIMD::NoiseType::ValueFractal);
		noiseRef->SetSeed(seed);
		this->name = name;
		this->tick = 0;
	}
	bool CheckMovePlatform(Pos pos, Facing toward);
	void MovePlatform(Pos pos, Facing toward);
	bool PushItems(std::vector<Pos>* itemsMoving, Facing toward, int pushesLeft); // bool is for whether the robot should move after a failed operation
	uint16_t ChangeItem(Pos pos, int quantity, uint16_t item);
	uint16_t ChangeRobot(Pos pos, int quantity);
	uint16_t ChangeLogic(Pos pos, int quantity, uint8_t logicType);
	void GenerateChunk(Pos pos);
	void GenerateOre(Pos pos);
	void Serialize(std::string filename);
	void Serialize() { if(name != "") Serialize(name); }
	void SerializeItemNames(std::string filename);
	void SerializeMisc(std::string filename);
	bool PlaceElement(Pos pos, uint16_t item);
	uint16_t ChangeElement(Pos pos, int quantity, uint16_t item);
	int ChangeInventory(uint16_t item, int quantity);
	void Deserialize(std::string filename);
	void DeserializeItemNames(std::string filename);
	void DeserializeMisc(std::string filename);
	void clear();
	void PasteSelection();
};
extern WorldSave world;
