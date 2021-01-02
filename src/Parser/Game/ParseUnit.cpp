#include "ParseUnit.h"
#include "Game.h"
#include "Game/Level.h"
#include "Game/Unit.h"
#include "GameUtils.h"
#include "Parser/Utils/ParseUtils.h"

namespace Parser
{
	using namespace rapidjson;
	using namespace std::literals;

	void parseUnit(Game& game, const Value& elem)
	{
		if (isValidString(elem, "class") == false)
		{
			return;
		}

		auto level = game.Resources().getLevel(getStringViewKey(elem, "level"));
		if (level == nullptr)
		{
			return;
		}

		auto class_ = level->getUnitClass(elem["class"sv].GetStringView());
		if (class_ == nullptr)
		{
			return;
		}

		auto mapPos = getVector2NumberKey<PairInt16, int16_t>(elem, "mapPosition");
		if (level->isMapCoordValid(mapPos) == false  ||
			level->hasUnit(mapPos) == true)
		{
			return;
		}

		auto direction = getUIntKey(elem, "direction");
		if (direction >= class_->Directions())
		{
			direction = 0;
		}

		auto unit = std::make_shared<Unit>(class_, mapPos, direction);

		unit->Points(getIntKey(elem, "points", class_->Points()));

		level->addUnit(unit);
	}
}
