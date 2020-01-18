#pragma once
#include "Facing.h"
#include "Pos.h"
#include "ReservedItems.h"
#include <vector>
class Robot{
public:
	Pos pos;												// 8 bytes
	uint16_t itemCarying = (uint16_t)ReservedItems::nothing;// 2 bytes
	bool stopped = false;									// 1 bit
	Facing facing = north;									// 1 byte
	uint8_t inbetweens = 0;
	void SetFacing(Facing direction);
	void Rotate(int r);
	bool Move();
	void Drop();
	void PushItems(std::vector<Pos>* itemsMoving, Facing toward, int pushesLeft);
	void DrawTile(int x, int y);
};
