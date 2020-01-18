#pragma once
#include "Pos.h"
#include <array>
#include "Tile.h"
#include "Constants.h"
struct WorldChunk {
	Pos chunkPos = Pos{ 0,0 };
	std::array<GroundTile, GC::chunkTileNum> tiles;
	GroundTile* GetTile(Pos worldPos)
	{
		int index = (worldPos.y << GC::chunkShift) | worldPos.x;
		return &tiles[index];
	}
};
