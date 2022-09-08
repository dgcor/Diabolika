#include "ParseUnit.h"
#include "Game/Game.h"
#include "Game/Level/Level.h"
#include "Game/Unit/Unit.h"
#include "Game/Utils/GameUtils.h"
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

		auto level = game.Resources().getLevel<Level>(getStringViewKey(elem, "level"));
		if (level == nullptr)
		{
			return;
		}

		auto class_ = level->getUnitClass(elem["class"sv].GetStringView());
		if (class_ == nullptr)
		{
			return;
		}

		auto boardPos = getVector2NumberKey<PairInt16, int16_t>(elem, "boardPosition");
		if (level->Board().isCoordValid(boardPos) == false  ||
			level->Board().hasUnit(boardPos) == true)
		{
			return;
		}

		auto direction = getUIntKey(elem, "direction");
		if (direction >= class_->Directions())
		{
			direction = 0;
		}

		auto unit = std::make_shared<Unit>(class_, boardPos, direction);

		unit->Points(getIntKey(elem, "points", class_->Points()));

		level->addUnit(unit);
	}
}
