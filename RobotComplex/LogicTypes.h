#pragma once
#include <cstdint>
#include <unordered_map>
#include <string>
static const std::unordered_map<uint8_t, std::string> logicTypes = {
	{1,"wire"},
	{2,"redirector"},
	{3,"gate"},
	{4,"belt"},
	{5,"shover"},
	{6,"wirebridge"},
	{7,"hub"},
	{128,"inverter"},
	{129,"pressureplate"},
	{130,"booster"},
	{131,"counter"},
	{132,"repeater"},
	{133,"comparer"},
	{134,"plusone"},
	{135,"toggle"}
};


enum LogicType : uint8_t {
	// Wire Logic
	wire = 1,
	redirector,
	gate,
	belt,
	shover,
	wirebridge,
	hub,

	// Unidirectional Output
	inverter = 128,
	pressureplate,
	booster,
	counter,
	repeater,
	comparer,
	plusone,
	toggle
};

const uint8_t UNI_DIR = 0b10000000;
/*
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
*/