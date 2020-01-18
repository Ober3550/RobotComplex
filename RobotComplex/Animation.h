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
/*Animation newAnimation;
	newAnimation.animationSpeed = 10;
	newAnimation.animationType = ping;
	newAnimation.frameWidth = 32;
	newAnimation.animationOffset = Pos{ 32,0 };*/