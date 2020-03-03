#pragma once
#include <cstdint>
#include <vector>
#include <string>
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
	"comparer",
	"plusone",
	"shover",
	"toggle"
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
	comparer = 10,
	plusone = 11,
	shover = 12,
	toggle = 13
};