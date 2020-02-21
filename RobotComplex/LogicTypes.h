#pragma once
#include <cstdint>
#include <vector>
static const std::vector<std::string> logicTypes =
{
	"wire",
	"pressureplate",
	"redirector",
	"inverter",
	"booster",
	"counter",
	"repeater",
	"gate",
	"belt",
	"wirebridge",
	"comparer"
};
enum LogicTypes : uint8_t {
	wire = 0,
	pressureplate = 1,
	redirector = 2,
	inverter = 3,
	booster = 4,
	counter = 5,
	repeater = 6,
	gate = 7,
	belt = 8,
	wirebridge = 9,
	comparer = 10
};