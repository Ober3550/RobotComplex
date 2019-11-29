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
		newChunk->chunkPos = pos.ChunkPosition();
		for (size_t i = 0; i < Gconstants::chunkTileNum; i++)
		{
			newChunk->tiles[i] = GroundTile{1};
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
	return nullptr;
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