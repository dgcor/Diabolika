#pragma once

#include "Game/Properties/UnitLink.h"
#include <string_view>

namespace GameUtils
{
	UnitLink getUnitLink(const std::string_view str, UnitLink val);
}
