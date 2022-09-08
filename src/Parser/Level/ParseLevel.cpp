#include "ParseLevel.h"
#include "Game/Game.h"
#include "Game/Level/Level.h"
#include "Game/Utils/GameUtils.h"
#include "Json/JsonUtils.h"
#include "ParseLevelOptions.h"
#include "ParseLevelScores.h"
#include "Parser/Drawables/ParseDrawable.h"
#include "Parser/ParseAction.h"
#include "Parser/Utils/ParseUtils.h"
#include "Utils/StringHash.h"

namespace Parser
{
	using namespace rapidjson;
	using namespace std::literals;

	void parseLevel(Game& game, const Value& elem)
	{
		auto id = getStringViewKey(elem, "id");
		auto level = game.Resources().getLevel<Level>(id);
		if (level == nullptr)
		{
			if (isValidId(id) == false)
			{
				return;
			}

			level = std::make_shared<Level>();

			game.Resources().addDrawable(id, level, true, getStringViewKey(elem, "resource"));
			game.Resources().setCurrentLevel(level);
			level->Id(id);
			level->setShader(game.Shaders().Level);

			parseDrawableProperties(game, elem, *level);
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
			parseLevelOptions(level->AllOptions(), elem["levels"sv]);
		}
		if (isValidArray(elem, "highScores") == true)
		{
			parseLevelScores(level->HighScores(), elem["highScores"sv]);
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
		auto explosion = game.Resources().getTexturePack(getStringViewKey(elem, "explosionTexturePack"));

		bool initLevel = getBoolKey(elem, "init");
		if (initLevel == false &&
			tiles == nullptr &&
			explodeCursor == nullptr &&
			explodePattern == nullptr &&
			explosion == nullptr)
		{
			return;
		}

		auto tileSize = (float)getUIntKey(elem, "tileSize");
		auto padding = getIntRectKey(elem, "padding");

		level->Init(tiles, explodeCursor, explodePattern, explosion, tileSize, padding);
	}
}
