#include "ParseUnitClass.h"
#include "Game.h"
#include "Game/Level.h"
#include "Game/UnitClass.h"
#include "Parser/ParseAction.h"
#include "Parser/Utils/ParseUtils.h"

namespace Parser
{
	using namespace rapidjson;
	using namespace std::literals;

	void parseExplodePattern(UnitClass& unitClass, const Value& elem)
	{
		std::vector<PairInt16> explodePattern;
		for (const auto& val : elem)
		{
			if (val.IsArray() == false)
			{
				continue;
			}
			if (val.Size() == 2 &&
				val[0].IsNumber() &&
				val[1].IsNumber())
			{
				auto explodePoint = getVector2iVal<PairInt16>(val);
				if (explodePoint.x == 0 && explodePoint.y == 0)
				{
					continue;
				}
				explodePattern.push_back(explodePoint);
			}
			else if (val[0].IsArray() == true)
			{
				parseExplodePattern(unitClass, val);
			}
		}
		if (explodePattern.empty() == false)
		{
			unitClass.addExplodePattern(explodePattern);
		}
	}

	void parseUnitClass(Game& game, const Value& elem)
	{
		auto level = game.Resources().getLevel(getStringViewKey(elem, "level"));
		if (level == nullptr)
		{
			return;
		}
		if (isValidString(elem, "id") == false)
		{
			return;
		}

		auto id = elem["id"sv].GetStringView();

		if (isValidId(id) == false ||
			level->getUnitClass(id) != nullptr)
		{
			return;
		}

		auto texturePack = game.Resources().getTexturePack(getStringViewKey(elem, "texturePack"));
		if (texturePack == nullptr)
		{
			return;
		}
		auto explosionTexturePack = game.Resources().getTexturePack(getStringViewKey(elem, "explosionTexturePack"));

		auto directions = getUIntKey(elem, "directions");
		auto unitClass = std::make_shared<UnitClass>(texturePack, explosionTexturePack, directions);

		unitClass->Points(getIntKey(elem, "points"));
		unitClass->Id(id);
		unitClass->Type(getStringViewKey(elem, "type"));
		unitClass->Spawns(getStringViewKey(elem, "spawns"));
		unitClass->SpawnCount(getStringKey(elem, "spawnCount", "1"));
		unitClass->SpawnArea(getIntRectKey(elem, "spawnArea"));
		unitClass->Links(getStringViewKey(elem, "links"));
		unitClass->LinkType(getUnitLinkKey(elem, "linkType"));
		unitClass->LinkCount(getStringKey(elem, "linkCount", "1"));

		if (isValidArray(elem, "explodePattern") == true)
		{
			parseExplodePattern(*unitClass.get(), elem["explodePattern"sv]);
			unitClass->BlastExpands(getBoolKey(elem, "blastExpands"));
		}
		else if (isValidString(elem, "explodePattern") == true &&
			elem["explodePattern"sv].GetStringView() == "random")
		{
			unitClass->ExplodeUnits(getStringKey(elem, "explodeUnits"));
		}

		if (elem.HasMember("onCreate"sv) == true)
		{
			unitClass->setCreateAction(getActionVal(game, elem["onCreate"sv]));
		}
		if (elem.HasMember("onDestroy"sv) == true)
		{
			unitClass->setDestroyAction(getActionVal(game, elem["onDestroy"sv]));
		}

		level->addUnitClass(id, unitClass);
	}
}
