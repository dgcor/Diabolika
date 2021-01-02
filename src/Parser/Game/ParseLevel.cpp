#include "ParseLevel.h"
#include "Game.h"
#include "Game/Level.h"
#include "GameUtils.h"
#include "Json/JsonUtils.h"
#include "Parser/ParseAction.h"
#include "Parser/Utils/ParseUtils.h"
#include "Utils/Utils.h"

namespace Parser
{
	using namespace rapidjson;
	using namespace std::literals;

	void parseLevelOptions(Level& level, const Value& elem)
	{
		LevelOptions options;
		std::vector<LevelOptions> allOptions;
		for (const auto& val : elem)
		{
			options.level = getUIntKey(val, "level", options.level + 1);
			options.numUnits = (uint16_t)getUIntKey(val, "units", options.numUnits);
			options.numDetonators = (uint16_t)getUIntKey(val, "exploders", options.numDetonators);
			options.maxDemons = (uint16_t)getUIntKey(val, "maxDemons", options.maxDemons);

			if (isValidArray(val, "insertUnits") == true)
			{
				options.insertUnits.clear();
				for (const auto& val2 : val["insertUnits"sv])
				{
					auto id = getStringKey(val2, "id");
					if (id.empty() == true)
					{
						continue;
					}
					auto count = getStringKey(val2, "count");
					options.insertUnits.push_back(std::make_pair(id, Formula(count)));
				}
			}
			if (isValidArray(val, "queueUnits") == true)
			{
				options.queueUnits.clear();
				for (const auto& val2 : val["queueUnits"sv])
				{
					auto id = getStringVal(val2);
					if (id.empty() == true)
					{
						continue;
					}
					options.queueUnits.push_back(id);
				}
			}
			if (isValidArray(val, "maxUnitsById") == true)
			{
				options.maxUnitsById.clear();
				for (const auto& val2 : val["maxUnitsById"sv])
				{
					auto id = getStringKey(val2, "id");
					if (id.empty() == true)
					{
						continue;
					}
					options.maxUnitsById[id] = (uint16_t)getUIntKey(val2, "count");
				}
			}
			if (isValidArray(val, "maxUnitsByType") == true)
			{
				options.maxUnitsByType.clear();
				for (const auto& val2 : val["maxUnitsByType"sv])
				{
					auto type = getStringKey(val2, "type");
					if (type.empty() == true)
					{
						continue;
					}
					options.maxUnitsByType[type] = (uint16_t)getUIntKey(val2, "count");
				}
			}
			allOptions.push_back(options);
		}
		level.setOptions(allOptions);
	}

	void parseLevelHighScores(Level& level, const Value& elem)
	{
		LevelScore score;
		std::vector<LevelScore> scores;
		for (const auto& val : elem)
		{
			score.name = getStringKey(val, "name");
			score.level = getUIntKey(val, "level");
			score.score = getUInt64Key(val, "score");
			scores.push_back(score);
		}
		level.setHighScores(scores);
	}

	void parseLevel(Game& game, const Value& elem)
	{
		auto id = getStringViewKey(elem, "id");
		auto level = game.Resources().getLevel(id);
		if (level == nullptr)
		{
			if (isValidId(id) == false)
			{
				return;
			}
			auto levelPtr = std::make_shared<Level>();
			game.Resources().addDrawable(id, levelPtr, true, getStringViewKey(elem, "resource"));
			level = levelPtr.get();
			game.Resources().setCurrentLevel(level);
			level->Id(id);
			level->setShader(game.Shaders().Level);

			auto anchor = getAnchorKey(elem, "anchor");
			level->setAnchor(anchor);
			auto size = level->Size();
			auto pos = getPositionKey(elem, "position", size, game.RefSize());
			if (getBoolKey(elem, "relativeCoords", true) == true &&
				game.RefSize() != game.DrawRegionSize())
			{
				GameUtils::setAnchorPosSize(anchor, pos, size, game.RefSize(), game.DrawRegionSize());
			}
			level->Position(pos);
		}

		if (elem.HasMember("name"sv) == true)
		{
			level->Name(getStringViewVal(elem["name"sv]));
		}
		if (elem.HasMember("scoreFormula"sv) == true)
		{
			level->setScoreFormula(getStringViewVal(elem["scoreFormula"sv]));
		}
		if (elem.HasMember("timeout"sv) == true)
		{
			level->setTimeout(getTimeVal(elem["timeout"sv]));
		}
		if (isValidArray(elem, "levels") == true)
		{
			parseLevelOptions(*level, elem["levels"sv]);
		}
		if (isValidArray(elem, "highScores") == true)
		{
			parseLevelHighScores(*level, elem["highScores"sv]);
		}

		if (elem.HasMember("onLeftClick"sv))
		{
			level->setAction(
				str2int16("leftClick"),
				getActionVal(game, elem["onLeftClick"sv])
			);
		}
		if (elem.HasMember("onRightClick"sv))
		{
			level->setAction(
				str2int16("rightClick"),
				getActionVal(game, elem["onRightClick"sv])
			);
		}
		if (elem.HasMember("onNewRound"sv))
		{
			level->setAction(
				str2int16("newRound"),
				getActionVal(game, elem["onNewRound"sv])
			);
		}
		if (elem.HasMember("onSelectedTileChange"sv))
		{
			level->setAction(
				str2int16("selectedTileChange"),
				getActionVal(game, elem["onSelectedTileChange"sv])
			);
		}
		if (elem.HasMember("onExplosion"sv))
		{
			level->setAction(
				str2int16("explosion"),
				getActionVal(game, elem["onExplosion"sv])
			);
		}
		if (elem.HasMember("onGameOver"sv))
		{
			level->setAction(
				str2int16("gameOver"),
				getActionVal(game, elem["onGameOver"sv])
			);
		}

		auto tiles = game.Resources().getTexturePack(getStringViewKey(elem, "tilesTexturePack"));
		auto explodeCursor = game.Resources().getTexturePack(getStringViewKey(elem, "explodeCursorTexturePack"));
		auto explodePattern = game.Resources().getTexturePack(getStringViewKey(elem, "explodePatternTexturePack"));

		TexturePackVariant explosionVar;
		if (isValidString(elem, "explosionTexturePack") == true)
		{
			auto obj = game.Resources().getTexturePack(elem["explosionTexturePack"sv].GetStringView());
			if (obj != nullptr)
			{
				explosionVar = std::move(obj);
			}
		}
		else if (isValidString(elem, "explosionCompositeTexture") == true)
		{
			auto obj = game.Resources().getCompositeTexture(elem["explosionCompositeTexture"sv].GetStringView());
			if (obj != nullptr)
			{
				explosionVar = std::move(obj);
			}
		}

		bool initLevel = getBoolKey(elem, "init");
		if (initLevel == false &&
			tiles == nullptr &&
			explodeCursor == nullptr &&
			explodePattern == nullptr &&
			explosionVar.holdsNullTexturePack() == true)
		{
			return;
		}

		auto tileSize = (float)getUIntKey(elem, "tileSize");
		auto padding = getIntRectKey(elem, "padding");

		level->Init(tiles, explodeCursor, explodePattern, explosionVar, tileSize, padding);
	}
}
