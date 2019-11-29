#pragma once
#include <SFML/Graphics.hpp>
#include "Pos.h"
class Animation
{
public:
	uint16_t textureRef;
	uint8_t frameWidth;
	uint8_t animationSpeed;
	Pos pos;
	Pos offset;
	int8_t animationPosition;
	void DrawAnimation();
};