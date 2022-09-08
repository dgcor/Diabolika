#pragma once

#include <cstdint>

enum class LevelState : int32_t
{
	GameOver,
	AddUnits,
	Detonate,
	Exploding,
	CheckStatus
};
