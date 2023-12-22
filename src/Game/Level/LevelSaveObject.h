#pragma once

#include "LevelStateObject.h"
#include "Game/Unit/Unit.h"
#include <memory>
#include <string>
#include <vector>

struct LevelSaveObject : public LevelStateObject
{
	std::vector<PairInt16> boardWalls;
	std::vector<std::shared_ptr<Unit>> boardUnits;
	std::vector<std::vector<std::shared_ptr<Unit>>> queues;
	std::vector<std::pair<PairInt16, PairInt16>> links;
	std::vector<PairInt16> explosions;
};
