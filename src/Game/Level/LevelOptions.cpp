#include "LevelOptions.h"

uint16_t LevelOptions::getMaxUnitsById(const std::string_view id) const
{
	auto it = maxUnitsById.find(id);
	if (it != maxUnitsById.end())
	{
		return it->second;
	}
	return 0;
}

uint16_t LevelOptions::getMaxUnitsByType(const std::string_view type) const
{
	auto it = maxUnitsByType.find(type);
	if (it != maxUnitsByType.end())
	{
		return it->second;
	}
	return 0;
}
