#include "LevelStats.h"
#include "Game/Unit/UnitClass.h"

uint16_t LevelStats::countUnitsById(const std::string_view id) const
{
	auto it = nunUnitsById.find(id);
	if (it != nunUnitsById.end())
	{
		return it->second;
	}
	return 0;
}

uint16_t LevelStats::countUnitsByType(const std::string_view type) const
{
	auto it = numUnitsByType.find(type);
	if (it != numUnitsByType.end())
	{
		return it->second;
	}
	return 0;
}

void LevelStats::incrementUnitCountById(const std::string_view id)
{
	auto it = nunUnitsById.insert(std::make_pair(id, 1));
	if (it.second == false)
	{
		it.first->second++;
	}
}

void LevelStats::decrementUnitCountById(const std::string_view id)
{
	auto it = nunUnitsById.insert(std::make_pair(id, 0));
	if (it.second == false &&
		it.first->second > 0)
	{
		it.first->second--;
	}
}

void LevelStats::incrementUnitCountByType(const std::string_view type)
{
	auto it = numUnitsByType.insert(std::make_pair(type, 1));
	if (it.second == false)
	{
		it.first->second++;
	}
}

void LevelStats::decrementUnitCountByType(const std::string_view type)
{
	auto it = numUnitsByType.insert(std::make_pair(type, 0));
	if (it.second == false &&
		it.first->second > 0)
	{
		it.first->second--;
	}
}

void LevelStats::decrementUnitCount(const UnitClass& unitClass)
{
	decrementUnitCountById(unitClass.Id());
	decrementUnitCountByType(unitClass.Type());
}

void LevelStats::incrementUnitCount(const UnitClass& unitClass)
{
	incrementUnitCountById(unitClass.Id());
	incrementUnitCountByType(unitClass.Type());
}
