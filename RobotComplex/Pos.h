#pragma once
#include <cstdint>
#include "Facing.h"
#include <cassert>
struct Pos {
public:
	int32_t x = 0, y = 0;
	uint64_t CoordToEncoded();
	static Pos EncodedToCoord(uint64_t input);
	Pos FacingPosition(Facing facing);
	Pos BehindPosition(Facing facing);
	Pos RelativePosition(int32_t dx, int32_t dy);
	Pos RelativePosition(Pos pos);
	Pos ChunkPosition();
	uint64_t ChunkEncoded();
	Pos InChunkPosition();
	bool operator==(Pos other);
	bool operator!=(Pos other);
	Pos operator>>(int other);
	Pos operator<<(int other);
	Pos operator+(Pos other);
	Pos operator-(Pos other);
};