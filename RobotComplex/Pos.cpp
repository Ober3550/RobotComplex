#include "Pos.h"
#include "Constants.h"

uint64_t Pos::CoordToEncoded()
{
	return (uint64_t(this->y) << 32) | uint32_t(this->x);
}
Pos Pos::EncodedToCoord(uint64_t input)
{
	return Pos{ int32_t(input & 0xFFFFFFFF), int32_t(input >> 32)};
}
Pos Pos::FacingPosition(Facing facing)
{
	switch (facing)
	{
	case north:
		return RelativePosition(0, -1);
	case east:
		return RelativePosition(1, 0);
	case south:
		return RelativePosition(0, 1);
	case west:
		return RelativePosition(-1, 0);
	default:
		assert(false);
		return *this;
	}
}
Pos Pos::BehindPosition(Facing facing)
{
	switch (facing)
	{
	case north:
		return RelativePosition(0, 1);
	case east:
		return RelativePosition(-1, 0);
	case south:
		return RelativePosition(0, -1);
	case west:
		return RelativePosition(1, 0);
	default:
		assert(false);
		return *this;
	}
}
Pos Pos::RelativePosition(int32_t dx, int32_t dy)
{
	Pos result{ (this->x + dx),(this->y + dy)};
	return result;
}
Pos Pos::RelativePosition(Pos other)
{
	return RelativePosition(other.x, other.y);
}
Pos Pos::ChunkPosition()
{
	Pos result{ this->x >> Gconstants::chunkShift, this->y >> Gconstants::chunkShift };
	return result;
}
uint64_t Pos::ChunkEncoded()
{
	return ChunkPosition().CoordToEncoded();
}
Pos Pos::InChunkPosition()
{
	Pos result{ this->x & Gconstants::chunkMask,this->y & Gconstants::chunkMask };
	return result;
}
bool Pos::operator==(Pos other)
{
	if (other.x == this->x && other.y == this->y)
		return true;
	else
		return false;
}
bool Pos::operator!=(Pos other)
{
	if (other.x == this->x && other.y == this->y)
		return false;
	else
		return true;
}
Pos Pos::operator>>(int shift)
{
	return Pos{ this->x >> shift, this->y >> shift };
}
Pos Pos::operator<<(int shift)
{
	return Pos{ this->x << shift, this->y << shift };
}
Pos Pos::operator+(Pos other)
{
	return Pos{ this->x + other.x, this->y + other.y };
}
Pos Pos::operator-(Pos other)
{
	return Pos{ this->x - other.x, this->y - other.y };
}