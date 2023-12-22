#pragma once

#include "Game/Drawables/Animation.h"
#include "Game/Unit/Unit.h"
#include <memory>

struct LevelCell
{
	uint32_t tileGroupIdx{ 0 };
	Animation tile;
	Animation anim;
	std::shared_ptr<Unit> unit;
	bool isWall{ false };
};
