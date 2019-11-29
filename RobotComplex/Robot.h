#pragma once
#include "Facing.h"
#include "Pos.h"
#include "ReservedItems.h"
class Robot{
public:
	Pos pos;												// 8 bytes
	uint16_t itemCarying = (uint16_t)ReservedItem::nothing;	// 2 bytes
	bool stopped = false;									// 1 bit
	Facing facing = north;									// 1 byte
	uint8_t inbetweens = 0;
	void SetFacing(Facing direction);
	void Rotate(int r);
	bool Move();
	void Drop();
	void TryCrafting(uint16_t item, Pos itemPos);
	void DrawTile(int x, int y);
};
