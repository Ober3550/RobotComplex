#include <unordered_map>
#include <unordered_set>
#include "Robot.h"
#include "LogicTile.h"
#include "WorldChunk.h"
#include "Pos.h"
#include "WorldSave.h"

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
		float* chunkNoise = noiseRef->GetSampledNoiseSet(currentPos.x, currentPos.y, 0, GC::chunkMask + 1, GC::chunkMask + 1, 1, 5);
		for (i = 0; i < GC::chunkTileNum; i++)
		{
			Pos currentPos = { (pos.x << GC::chunkShift) + (i & GC::chunkMask), (pos.y << GC::chunkShift) + (i / (GC::chunkMask + 1)) };
			// For some reason mapping x to y fixes the perlin sampling
			float normalized = ((chunkNoise[(currentPos.x & GC::chunkMask) * 32 + (currentPos.y & GC::chunkMask)] + 1.0f) / 2.0f);
			newChunk->tiles[i] = GroundTile{(uint8_t)(normalized * 512)};
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
	if (LogicTile * *temp = world.logictiles.GetValue(pos.CoordToEncoded()))
		return *temp;
	else
		return nullptr;
}
LogicTile* WorldSave::GetLogicTile(uint64_t encodedPos)
{
	if (LogicTile * *temp = world.logictiles.GetValue(encodedPos))
		return *temp;
	else
		return nullptr;
}
CraftingProcess* WorldSave::GetCrafting(Pos pos)
{
	return world.craftingQueue.GetValue(pos.CoordToEncoded());
}
CraftingProcess* WorldSave::GetCrafting(uint64_t encodedPos)
{
	return world.craftingQueue.GetValue(encodedPos);
}
bool WorldSave::ChangeItem(Pos pos, uint16_t item, int quantity)
{
	uint64_t itemPos = pos.CoordToEncoded();
	if (ItemTile* currentItem = world.GetItemTile(itemPos))
	{
		if (currentItem->itemTile != item && currentItem->itemTile != 0)
			return false;
		else
		{
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
		world.updateQueueD.insert(pos.CoordToEncoded());
		return true;
	}
	else
	{
		if (quantity <= 0)
			return false;
		ItemTile* newItem = new ItemTile();
		newItem->itemTile = item;
		newItem->quantity = quantity;
		world.items[pos.CoordToEncoded()] = *newItem;
		world.updateQueueD.insert(pos.CoordToEncoded());
		return true;
	}
}
bool WorldSave::PushItems(std::vector<Pos>* itemsMoving, Facing toward, int pushesLeft)
{
	Pos prevPos = itemsMoving->back();
	Pos nextPos = prevPos.FacingPosition(toward);
	// If there is already a different item moving to the position specified don't continue
	if (!world.itemMovingTo.contains(nextPos.CoordToEncoded()) && !world.prevItemMovingTo.contains(nextPos.CoordToEncoded()))
	{
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
					if (pushesLeft == 1 && nextItem->quantity == UINT8_MAX)
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
		if (uint16_t* currentPlatform = world.platforms.GetValue(pos.CoordToEncoded()))
		{
			// If this platform is moving to a position that another is already moving to exit
			if (!world.platformMovingTo.contains(nextPos.CoordToEncoded()))
			{
				// Add this current platform element to the connected set
				world.connectedPlatform.insert(pos.CoordToEncoded());
				if (uint16_t* nextPlatform = world.platforms.GetValue(nextPos.CoordToEncoded()))
				{
					if (*currentPlatform != *nextPlatform)
					{
						return false;
					}
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