#pragma once
#include "Facing.h"
#include "Pos.h"
#include "ReservedItems.h"
#include <vector>
#include "SpriteVector.h"
class Robot {
public:
	uint8_t robotType;
	Facing facing = north;									// 1 byte
	bool stopped = false;									// 1 byte
	int MemorySize() { return 10; };
	void SetFacing(Facing direction);
	void Rotate(int r);
	bool Move(Pos pos);
	void DrawTile(SpriteVector* appendTo, int x, int y, float s, uint8_t flags, sf::Color color);
	Robot* Copy()
	{
		Robot* newRobot = new Robot();
		newRobot->facing = this->facing;
		newRobot->stopped = this->stopped;
		return newRobot;
	}
	std::string GetTooltip()
	{
		return "This is a Robot";
	}
};
