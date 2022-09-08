#pragma once

#include "Game/Formula.h"
#include "Utils/UnorderedStringMap.h"
#include <vector>

struct LevelOptions
{
	uint32_t level{ 0 };
	uint16_t numUnits{ 8 };
	uint16_t numDetonators{ 2 };
	uint16_t maxDemons{ 10 };
	UnorderedStringMap<uint16_t> maxUnitsById;
	UnorderedStringMap<uint16_t> maxUnitsByType;
	std::vector<std::pair<std::string, Formula>> insertUnits;
	std::vector<std::string> queueUnits;

	uint16_t getMaxUnitsById(const std::string_view id) const;

	uint16_t getMaxUnitsByType(const std::string_view type) const;
};
