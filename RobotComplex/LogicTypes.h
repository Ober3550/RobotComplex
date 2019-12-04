#pragma once
#include <cstdint>
enum LogicTypes : uint8_t { 
	wire = 1,
	pressureplate = 2,
	redirector = 3, 
	inverter = 4,
	booster = 5,
	counter = 6,
	repeater = 7,
	holder = 8
};