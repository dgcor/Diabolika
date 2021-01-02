#include "GameUtilsGame.h"
#include "Utils/Utils.h"

namespace GameUtils
{
	UnitLink getUnitLink(const std::string_view str, UnitLink val)
	{
		switch (str2int16(Utils::toLower(str)))
		{
		case str2int16("none"):
			return UnitLink::None;
		case str2int16("unidirectional"):
			return UnitLink::Unidirectional;
		case str2int16("bidirectional"):
			return UnitLink::Bidirectional;
		case str2int16("unit"):
			return UnitLink::Unit;
		default:
			return val;
		}
	}
}
