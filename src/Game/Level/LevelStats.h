#pragma once

#include <cstdint>
#include "Utils/UnorderedStringMap.h"

class UnitClass;

class LevelStats
{
private:
	UnorderedStringMap<uint16_t> nunUnitsById;
	UnorderedStringMap<uint16_t> numUnitsByType;

	void incrementUnitCountById(const std::string_view id);
	void decrementUnitCountById(const std::string_view id);
	void incrementUnitCountByType(const std::string_view type);
	void decrementUnitCountByType(const std::string_view type);

public:
	uint16_t countUnitsById(const std::string_view id) const;

	uint16_t countUnitsByType(const std::string_view type) const;

	void decrementUnitCount(const UnitClass& unitClass);

	void incrementUnitCount(const UnitClass& unitClass);
};
