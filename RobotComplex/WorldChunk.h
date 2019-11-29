#pragma once
#include "Pos.h"
#include <array>
#include "Tile.h"
#include "Constants.h"
struct WorldChunk {
	Pos chunkPos = Pos{ 0,0 };
	std::array<GroundTile, Gconstants::chunkTileNum> tiles;
	GroundTile* GetTile(Pos worldPos)
	{
		int index = worldPos.y << Gconstants::chunkShift | worldPos.x;
		return &tiles[index];
	}
};
