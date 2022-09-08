#pragma once

#include <cstdint>
#include <string>

struct LevelScore
{
	std::string name;
	uint32_t level{ 0 };
	uint64_t score{ 0 };
};
