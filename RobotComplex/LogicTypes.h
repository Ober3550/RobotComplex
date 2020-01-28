#pragma once
#include <cstdint>
static const std::vector<std::string> logicTypes =
{
	"wire",
	"pressureplate",
	"redirector",
	"inverter",
	"booster",
	"counter",
	"repeater",
	"holder",
	"belt",
	"wirebridge"
};
enum LogicTypes : uint8_t { 
	wire = 1,
	pressureplate = 2,
	redirector = 3, 
	inverter = 4,
	booster = 5,
	counter = 6,
	repeater = 7,
	holder = 8,
	belt = 9,
	wirebridge = 10
};