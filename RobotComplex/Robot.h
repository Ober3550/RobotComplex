#pragma once
#include "Facing.h"
#include "Pos.h"
#include "ReservedItems.h"
#include "ParentTile.h"
#include <vector>
#include "SpriteVector.h"
class Robot : public ParentTile {
public:
	Pos pos;												// 8 bytes
	Facing facing = north;									// 1 byte
	bool stopped = false;									// 1 byte
	int MemorySize() { return 10; };
	void SetFacing(Facing direction);
	void Rotate(int r);
	bool Move();
	void DrawTile(SpriteVector* appendTo, int x, int y, float s);
	Robot* Copy()
	{
		Robot* newRobot = new Robot();
		newRobot->pos = this->pos;
		newRobot->facing = this->facing;
		newRobot->stopped = this->stopped;
		return newRobot;
	}
	std::string GetTooltip()
	{
		return "This is a Robot";
	}
	virtual void Serialize(std::ofstream*);
	virtual void Deserialize(std::ifstream*);
};
