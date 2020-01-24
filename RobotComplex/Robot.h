#pragma once
#include "Facing.h"
#include "Pos.h"
#include "ReservedItems.h"
#include <vector>
class Robot{
public:
	Pos pos;												// 8 bytes
	Facing facing = north;									// 1 byte
	bool stopped = false;									// 1 byte
	void SetFacing(Facing direction);
	void Rotate(int r);
	bool Move();
	void DrawTile(int x, int y, float s);
};
