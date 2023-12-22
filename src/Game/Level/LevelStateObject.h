#pragma once

#include "LevelState.h"
#include "Utils/PairXY.h"

struct LevelStateObject
{
	LevelState state{ LevelState::GameOver };

	uint32_t level{ 0 };
	uint64_t score{ 0 };
	uint32_t chain{ 0 };

	uint16_t walls{ 0 };
	uint16_t units{ 0 };
	uint16_t detonators{ 0 };

	PairInt16 selectedPosition;
};
