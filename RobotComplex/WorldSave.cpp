#include <unordered_map>
#include <unordered_set>
#include "Robot.h"
#include "LogicTile.h"
#include "WorldChunk.h"
#include "Pos.h"
#include "WorldSave.h"
#include <typeinfo>
#include "ProgramData.h"
#include "Oregen.h"
#include "FindInVector.h"
#include <math.h>

void WorldSave::GenerateChunk(Pos pos)
{
	uint64_t encoded = pos.CoordToEncoded();
	auto val = worldChunks.try_emplace(encoded);
	bool isNewlyPlaced = val.second;
	if (isNewlyPlaced)
	{
		auto newChunk = &val.first->second;
		newChunk->chunkPos = pos;
		int i = 0;
		Pos currentPos = { (pos.x << GC::chunkShift) + (i & GC::chunkMask), (pos.y << GC::chunkShift) + (i / (GC::chunkMask + 1)) };
		float* chunkNoise = noiseRef->GetSampledNoiseSet(currentPos.x, currentPos.y, 0, GC::chunkMask + 1, GC::chunkMask + 1, 1, 1);
		for (i = 0; i < GC::chunkTileNum; i++)
		{
			Pos currentPos = { (pos.x << GC::chunkShift) + (i & GC::chunkMask), (pos.y << GC::chunkShift) + (i / (GC::chunkMask + 1)) };
			// For some reason mapping x to y fixes the perlin sampling
			float normalized = ((chunkNoise[(currentPos.x & GC::chunkMask) * 32 + (currentPos.y & GC::chunkMask)] + 1.0f) / 2.0f);
			newChunk->tiles[i] = GroundTile{(uint8_t)(normalized * 512)};
		}
		GenerateOre(pos);
	}
}
void WorldSave::GenerateOre(Pos pos)
{
	for (size_t i = 0; i < ores.size(); i++)
	{
		Pos region = Pos(pos);
		Pos adjust = Pos{ 0,0 };
		if (pos.x < 0)
			adjust.x = -1;
		if (pos.y < 0)
			adjust.y = -1;
		region = region / oreRarities[i];
		// Some jank to get the random seed to work properly
		srand(((uint32_t(region.y + adjust.y) << 16) | uint16_t(region.x + adjust.x)) ^ (i * 41 % 17));
		Pos generate = Pos{ rand(),rand() };
		generate = (generate % (oreRarities[i] * GC::chunkSize)) + ((region + adjust) * oreRarities[i]) * GC::chunkSize;
		if (generate.ChunkPosition() == pos)
		{
			auto item = program.itemLookups.find(ores[i]);
			if (item != program.itemLookups.end())
			{
				uint16_t itemNumber = item->second;
				ItemTile newItem = ItemTile();
				newItem.itemTile = itemNumber;
				int size = rand() % 10 + 10;
				for (int x = generate.x - size; x < generate.x + size; x++)
				{
					for (int y = generate.y - size; y < generate.y + size; y++)
					{
						int distance = pow(generate.x - x, 2) + pow(generate.y - y, 2);
						int quantity = size - sqrt(distance);
						if (quantity < 0)
							quantity = 0;
						if (quantity > 0)
						{
							newItem.quantity = quantity * 7;
							Pos newPos = Pos{ x, y };
							world.items.insert({ newPos.CoordToEncoded(), newItem });
						}
					}
				}
			}
		}
	}
	
}
GroundTile* WorldSave::GetGroundTile(Pos pos)
{
	if (WorldChunk * chunk = worldChunks.GetValue(pos.ChunkEncoded()))
	{
		Pos inChunk = pos.InChunkPosition();
		return chunk->GetTile(inChunk);
	}
	else
	{
		GenerateChunk(pos.ChunkPosition());
		if (WorldChunk * chunk = worldChunks.GetValue(pos.ChunkEncoded()))
		{
			Pos inChunk = pos.InChunkPosition();
			return chunk->GetTile(inChunk);
		}
		// Chunk failed to generate
		assert(false);
		return nullptr;
	}
}
ItemTile* WorldSave::GetItemTile(Pos pos)
{
	return world.items.GetValue(pos.CoordToEncoded());
}
ItemTile* WorldSave::GetItemTile(uint64_t encodedPos)
{
	return world.items.GetValue(encodedPos);
}
Robot* WorldSave::GetRobot(Pos pos)
{
	return world.robots.GetValue(pos.CoordToEncoded());
}
Robot* WorldSave::GetRobot(uint64_t encodedPos)
{
	return world.robots.GetValue(encodedPos);
}
LogicTile* WorldSave::GetLogicTile(Pos pos)
{
	return world.logicTiles.GetValue(pos.CoordToEncoded());
}
LogicTile* WorldSave::GetLogicTile(uint64_t encodedPos)
{
	return world.logicTiles.GetValue(encodedPos);
}
CraftingProcess* WorldSave::GetCrafting(Pos pos)
{
	return world.craftingQueue.GetValue(pos.CoordToEncoded());
}
CraftingProcess* WorldSave::GetCrafting(uint64_t encodedPos)
{
	return world.craftingQueue.GetValue(encodedPos);
}
uint16_t WorldSave::ChangeItem(Pos pos, int quantity, uint16_t item)
{
	uint64_t itemPos = pos.CoordToEncoded();
	if (ItemTile* currentItem = world.GetItemTile(itemPos))
	{
		if (item != 0 && currentItem->itemTile != item && currentItem->itemTile != 0)
			return false;
		else
		{
			if(quantity > 0)
				currentItem->itemTile = item;
			if (currentItem->quantity < quantity && quantity < 0 || int(currentItem->quantity) + quantity > int(UINT8_MAX))
				return false;
			else
				currentItem->quantity += quantity;
		}
		if (currentItem->quantity == 0)
		{
			world.items.erase(itemPos);
		}
		if (!world.updateQueueD.contains(pos.CoordToEncoded()))
		{
			if (LogicTile* logicTile = world.GetLogicTile(itemPos))
			{
				logicTile->DoItemLogic(Pos::EncodedToCoord(itemPos));
			}
		}
		return currentItem->itemTile;
	}
	else
	{
		if (quantity <= 0)
			return false;
		ItemTile* newItem = new ItemTile();
		newItem->itemTile = item;
		newItem->quantity = quantity;
		world.items[pos.CoordToEncoded()] = *newItem;
		if (!world.updateQueueD.contains(pos.CoordToEncoded()))
		{
			if (LogicTile* logicTile = world.GetLogicTile(itemPos))
			{
				logicTile->DoItemLogic(Pos::EncodedToCoord(itemPos));
			}
		}
		return true;
	}
}
bool WorldSave::PushItems(std::vector<Pos>* itemsMoving, Facing toward, int pushesLeft)
{
	Pos prevPos = itemsMoving->back();
	Pos nextPos = prevPos.FacingPosition(toward);
	// If there is already a different item moving to the position specified don't continue
	// && !world.prevItemMovingTo.contains(nextPos.CoordToEncoded())
	if (!world.itemMovingTo.contains(nextPos.CoordToEncoded()))
	{
		if (LogicTile* logic = world.GetLogicTile(nextPos))
		{
			if (logic->logicType == gate)
			{
				if (!logic->signal)
				{
					itemsMoving->clear();
					return false;
				}
			}
		}
		ItemTile* prevItem = world.GetItemTile(prevPos);
		if (pushesLeft == 0)
		{
			// There is an item at the front of the stack that isn't the same type
			if (ItemTile* nextItem = world.GetItemTile(nextPos))
			{
				if (prevItem->itemTile != nextItem->itemTile && nextItem->quantity > 1)
				{
					itemsMoving->pop_back();
				}
			}
			return true;
		}
		else
		{
			if (ItemTile* nextItem = world.GetItemTile(nextPos))
			{
				if (prevItem->itemTile != nextItem->itemTile && nextItem->quantity > 1)
				{
					return true;
				}
				else
				{
					// If the item infront of the stack is the same type but is already 'full' don't push the stack
					if (pushesLeft == 1 && nextItem->quantity == 1)
					{
						itemsMoving->clear();
					}
					else
					{
						itemsMoving->emplace_back(nextPos);
						PushItems(itemsMoving, toward, pushesLeft - 1);
					}
				}
			}
			else
			{
				itemsMoving->emplace_back(nextPos);
				return true;
			}
		}
	}
	else
	{
		return false;
	}
}

bool WorldSave::CheckMovePlatform(Pos pos, Facing toward)
{
	Pos nextPos = pos.FacingPosition(toward);
	// Check next ground position (also activates chunk generation outside of camera view)
	if (GroundTile* ground = world.GetGroundTile(nextPos))
	{
		if (LogicTile* currentPlatform = world.GetLogicTile(pos.CoordToEncoded()))
		{
			// If this platform is moving to a position that another is already moving to exit
			if (!world.platformMovingTo.contains(nextPos.CoordToEncoded()))
			{
				// Add this current platform element to the connected set
				world.connectedPlatform.insert(pos.CoordToEncoded());
				if (LogicTile* nextPlatform = world.GetLogicTile(nextPos.CoordToEncoded()))
				{
					/*
					if (currentPlatform != nextPlatform)
					{
						return false;
					}
					*/
				}
				for (int i = 0; i < 4; i++)
				{
					// Check neighbours toward direction of travel first so that a failure can be detected asap
					Pos neighbour = pos.FacingPosition(Pos::RelativeFacing(toward, i));
					// Only check if a neighbour isn't already part of the check
					if (!world.connectedPlatform.contains(neighbour.CoordToEncoded()))
					{
						CheckMovePlatform(neighbour, toward);
					}
				}
			}
			else
			{
				return false;
			}
		}
	}
	return true;
}

void WorldSave::MovePlatform(Pos pos, Facing toward)
{
	world.moving = true;
	world.connectedPlatform.clear();
	if (CheckMovePlatform(pos, toward))
	{
		for (uint64_t plate : world.connectedPlatform)
		{
			world.platformMovingTo.insert(Pos::EncodedToCoord(plate).FacingPosition(toward).CoordToEncoded());
			world.nextPlatforms.insert({ plate,toward });
		}
	}
}

uint16_t WorldSave::ChangeLogic(Pos pos, int quantity, uint8_t logicType)
{
	if (LogicTile* logic = world.GetLogicTile(pos))
	{
		if (logicType != 0 && logic->logicType != logicType)
			return false;
		if (logic->quantity + quantity < 0 || logic->quantity + quantity > UINT8_MAX)
			return false;
		else
		{
			if (logic->color != program.placeColor 
			&& logic->logicType == wire
			&& quantity > 0)
			{
				logic->logicType = wirebridge;
				logic->color2 = program.placeColor;
				for (int i = 0; i < 4; i++)
				{
					Pos neighbourPosition = pos.FacingPosition((Facing)i);
					if (LogicTile* neighbour = world.GetLogicTile(neighbourPosition))
					{
						if (neighbour->color == logic->color)
						{
							logic->facing = (Facing)i;
							break;
						}
					}
				}
				world.updateNext.insert({ pos.CoordToEncoded(),1 });
			}
			
			logic->quantity += quantity;
			if (logic->quantity == 1 
				&& logic->logicType == wirebridge)
			{
				logic->logicType = wire;
				logic->color2 = logic->color;
				world.updateNext.insert({ pos.CoordToEncoded(),1 });
			}
		}
		if (logic->quantity == 0)
		{
			if (program.selectedLogicTile == logic)
				program.selectedLogicTile = nullptr;
			world.logicTiles.erase(pos.CoordToEncoded());
			for (int i = 0; i < 4; i++)
			{
				world.updateNext.insert({ pos.FacingPosition(Facing(i)).CoordToEncoded(),1 });
			}
		}
		return logic->logicType + program.itemsEnd;
	}
	else
	{
		if (quantity < 0)
			return false;
		if (quantity > 0)
		{
			LogicTile logicPlace = LogicTile(logicType);
			logicPlace.color = program.placeColor;
			logicPlace.color2 = program.placeColor;
			logicPlace.facing = program.placeRotation;
			logicPlace.quantity = quantity;
			world.logicTiles.insert({ pos.CoordToEncoded(),logicPlace });
			world.updateNext.insert({ pos.CoordToEncoded(),1 });
			for (int i = 0; i < 4; i++)
			{
				world.updateNext.insert({ pos.FacingPosition(Facing(i)).CoordToEncoded(),1 });
			}
			return logicPlace.logicType + program.itemsEnd;
		}
	}
	return false;
}

uint16_t WorldSave::ChangeRobot(Pos pos, int quantity)
{
	if (Robot* robot = world.GetRobot(pos))
	{
		if (quantity > 0)
			return false;
		if(robot == program.selectedRobot)
			program.selectedRobot = nullptr;
		world.robots.erase(pos.CoordToEncoded());
		return program.itemsEnd + GC::MAXLOGIC;
	}
	else
	{
		if (quantity > 0)
		{
			Robot newRobot;
			newRobot.stopped = true;
			world.robots.insert({ pos.CoordToEncoded(),newRobot });
		}
		else if (quantity < 0)
			return false;
	}
	return true;
}

bool WorldSave::PlaceElement(Pos pos, uint16_t item)
{
	if (item <= program.itemsEnd)
	{
		if(world.ChangeItem(pos, 1, item))
			return true;
		return false;
	}
	else if (item < program.itemsEnd + GC::MAXLOGIC)
	{
		if (world.ChangeLogic(pos, 1, item - program.itemsEnd))
			return true;
		return false;
	}
	else if(item == program.itemsEnd + GC::MAXLOGIC)
	{
		if (world.ChangeRobot(pos, 1))
			return true;
		return false;
	}
}

uint16_t WorldSave::ChangeElement(Pos pos, int quantity, uint16_t item)
{
	if (quantity > 0)
	{
		return PlaceElement(pos, item);
	}
	else if (quantity < 0)
	{
		uint16_t element;
		if (item <= program.itemsEnd)
		{
			return world.ChangeItem(pos, quantity, item);
		}
		else if (item < program.itemsEnd + GC::MAXLOGIC)
		{
			return world.ChangeLogic(pos, quantity, item);
		}
		else if(item == program.itemsEnd + GC::MAXLOGIC)
		{
			return world.ChangeRobot(pos, quantity);
		}
	}
	return true;
}

int WorldSave::ChangeInventory(uint16_t item, int quantity)
{
	if (quantity > 0)
	{
		for (auto& kv : program.hotbar)
		{
			if (kv.second.itemTile == item)
			{
				int subQuantity = quantity;
				if (kv.second.quantity + subQuantity > 255)
					subQuantity = 255 - kv.second.quantity;
				kv.second.quantity += subQuantity;
				quantity -= subQuantity;
			}
			if (quantity <= 0)
				break;
		}
		if (quantity > 0)
		{
			for (int j = 1; j >= 0; j--)
			{
				for (int i = 0; i < 10; i++)
				{
					auto kv = program.hotbar.find(SmallPos{ (uint8_t)i,(uint8_t)j });
					if (kv == program.hotbar.end())
					{
						ItemTile newItem = ItemTile(item);
						int subQuantity = quantity;
						if (subQuantity > 255)
							subQuantity = 255;
						newItem.quantity = subQuantity;
						quantity -= subQuantity;
						program.hotbar.insert({ SmallPos{(uint8_t)i,(uint8_t)j},newItem });
					}
					if (quantity == 0)
						break;
				}
				if (quantity == 0)
					break;
			}
		}
	}
	else if (quantity < 0)
	{
		std::vector<SmallPos> removeList;
		for (auto kv : program.hotbar)
		{
			if (kv.second.itemTile == item)
			{
				auto element = program.hotbar.find(kv.first);
				if (element != program.hotbar.end())
				{
					int subQuantity = quantity;
					if ((int)element->second.quantity + subQuantity < 0)
						subQuantity = -(int)element->second.quantity;
					element->second.quantity += subQuantity;
					if (element->second.quantity == 0)
						removeList.emplace_back(element->first);
					quantity -= subQuantity;
					if (quantity == 0)
						break;
				}
			}
		}
		for (auto k : removeList)
		{
			program.hotbar.erase(k);
		}
	}
	return quantity;
}