#pragma once
#include <cstdint>
#include "Facing.h"
struct LogicPack
{
	Facing facing : 2;
	uint8_t colorClass : 3;
};