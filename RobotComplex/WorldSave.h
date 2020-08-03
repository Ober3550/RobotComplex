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
	MySet<uint64_t> updateItemsNext;	// Queue for item updates next tick
	bool moving = false;
	std::string name;
	uint64_t tick;
	int seed = 0;
	// Variables for keeping elements and data structure organised between versions/changes
	MyMap<uint16_t, uint16_t> oldItemNewItem;	// A map from the old items to new items
	MyMap<uint8_t, uint8_t> oldLogicNewLogic;
	std::vector<uint16_t> oldLogicSize;

	std::vector<std::string> unlockedTechnologies;
	TechProto   currentTechnology;
	bool		techCompleted = false;
	MyMap<uint16_t, int> resourcesDelivered;

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
	WorldSave();
	WorldSave(std::string name);
	bool CheckMovePlatform(Pos pos, Facing toward);
	void MovePlatform(Pos pos, Facing toward);
	bool PushItems(std::vector<Pos>* itemsMoving, Facing toward, int pushesLeft); // bool is for whether the robot should move after a failed operation
	BigItem ChangeItem     (Pos pos,	  BigItem item);
	BigItem ChangeRobot    (Pos pos,      BigItem item);
	BigItem ChangeLogic    (Pos pos,      BigItem item);
	BigItem ChangeElement  (Pos pos,	  BigItem item);
	BigItem ChangeInventory(SmallPos pos, BigItem item);
	void GenerateChunk(Pos pos);
	void DrawChunk(Pos pos, sf::Vector2f startPos);
	void GenerateOre(Pos pos);
	void Serialize(std::string filename);
	void Serialize() { if(name != "") Serialize(name); }
	void SerializeItemNames(std::string filename);
	void SerializeMisc(std::string filename);
	void Deserialize(std::string filename);
	void DeserializeItemNames(std::string filename);
	void DeserializeMisc(std::string filename);
	void clear();
	void PasteSelection();
	void FindNextTechnology();
};
extern WorldSave world;
