#pragma once

#include <deque>
#include "Game/Unit/Unit.h"
#include <memory>

struct LevelUnitQueue
{
	std::shared_ptr<Unit> unit;
	std::deque<std::shared_ptr<Unit>> queue;
};
