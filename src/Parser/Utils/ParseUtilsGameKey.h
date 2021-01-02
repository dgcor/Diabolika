#pragma once

#include "ParseUtilsGameVal.h"
#include <string_view>

namespace Parser
{
	UnitLink getUnitLinkKey(const rapidjson::Value& elem,
		const std::string_view key, UnitLink val = UnitLink::None);
}
