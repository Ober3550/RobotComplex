#pragma once
#include <SFML/Graphics.hpp>
#include "Pos.h"
#include "AnimationType.h"
class Animation
{
public:
	uint16_t textureRef;
	uint8_t frameWidth;
	uint8_t animationSpeed;
	AnimationType animationType;
	Pos animationOffset;
};