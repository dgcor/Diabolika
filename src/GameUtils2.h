#pragma once

#include "Game/GameProperties.h"
#include <string_view>

namespace GameUtils
{
	UnitLink getUnitLink(const std::string_view str, UnitLink val);
}
