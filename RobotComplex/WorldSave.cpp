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
			newChunk->tiles[i] = GroundTile{(uint8_t)(normalized * 256)};
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
			if (currentItem->quantity < quantity && quantity < 0 || int(currentItem->quantity) + quantity > int(GC::tileItemLimit))
				return false;
			else
				currentItem->quantity += quantity;
		}
		if (currentItem->quantity == 0)
		{
			world.items.erase(itemPos);
		}
		if (LogicTile* logicTile = world.GetLogicTile(pos))
		{
			logicTile->DoItemLogic();
		}
		return true;
	}
	else
	{
		if (quantity <= 0)
			return false;
		ItemTile* newItem = new ItemTile();
		newItem->itemTile = item;
		newItem->quantity = quantity;
		if (LogicTile* logicTile = world.GetLogicTile(pos))
		{
			logicTile->DoItemLogic();
		}
		world.items[pos.CoordToEncoded()] = *newItem;
		return true;
	}
}