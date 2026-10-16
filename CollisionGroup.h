#pragma once
#include <cstdint>

enum CollisionGroup : uint64_t {
	PLAYER = 0b0001,
	LEVEL = 0b0010,
	SENSOR = 0b0100,
};