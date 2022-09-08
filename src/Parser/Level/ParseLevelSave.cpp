#include "ParseLevelSave.h"
#include "Game/Level/Level.h"
#include "Parser/Utils/ParseUtils.h"

namespace Parser
{
	using namespace rapidjson;
	using namespace std::literals;

	std::shared_ptr<Unit> parseLevelSaveUnit(const Level& level, const Value& elem)
	{
		auto unitClass = level.getUnitClass(getStringKey(elem, "unit"));
		if (unitClass != nullptr)
		{
			return std::make_shared<Unit>(
				unitClass,
				getVector2iKey<PairInt16>(elem, "position", { -1, -1 }),
				getUIntKey(elem, "direction"));
		}
		return nullptr;
	}

	LevelSaveObject parseLevelSaveObj(const Level& level, const Value& elem)
	{
		LevelSaveObject levelSave;

		levelSave.state = (LevelState)getIntKey(elem, "state");
		levelSave.level = getUIntKey(elem, "level");
		levelSave.score = getUInt64Key(elem, "score");
		levelSave.chain = getUIntKey(elem, "chain");
		levelSave.units = getUIntKey(elem, "units");
		levelSave.detonators = getUIntKey(elem, "detonators");
		levelSave.selectedPosition = getVector2iKey<PairInt16>(elem, "selectedPosition");

		if (isValidArray(elem, "board") == true)
		{
			for (const auto& val : elem["board"sv])
			{
				auto unit = parseLevelSaveUnit(level, val);
				if (unit == nullptr)
				{
					continue;
				}
				levelSave.boardUnits.push_back(unit);

				if (val.HasMember("linkedTo"sv) == true)
				{
					levelSave.links.push_back(
						std::make_pair(
							unit->BoardPosition(),
							getVector2iVal<PairInt16>(val["linkedTo"])));
				}
			}
		}

		if (isValidArray(elem, "queue") == true)
		{
			int16_t queueIdx = -1;
			for (const auto& queueElem : elem["queue"sv])
			{
				levelSave.queues.push_back({});

				if (queueElem.IsArray() == false)
				{
					continue;
				}
				for (const auto& val : queueElem)
				{
					auto unit = parseLevelSaveUnit(level, val);
					if (unit == nullptr)
					{
						continue;
					}
					unit->Position({ -1, -1 });
					levelSave.queues.back().push_back(unit);

					if (val.HasMember("linkedTo"sv) == true)
					{
						levelSave.links.push_back(
							std::make_pair(
								PairInt16(queueIdx, (int16_t)levelSave.queues.back().size() - 1),
								getVector2iVal<PairInt16>(val["linkedTo"])));
					}
				}
				queueIdx--;
			}
		}

		if (isValidArray(elem, "explosions") == true)
		{
			for (const auto& val : elem["explosions"sv])
			{
				levelSave.explosions.push_back(getVector2iVal<PairInt16>(val));
			}
		}

		return levelSave;
	}
}
