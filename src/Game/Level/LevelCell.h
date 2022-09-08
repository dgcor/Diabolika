#pragma once

#include "Game/Drawables/Animation.h"
#include "Game/Unit/Unit.h"
#include <memory>

struct LevelCell
{
	Image tile;
	Animation anim;
	std::shared_ptr<Unit> unit;
};
