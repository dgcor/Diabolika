#include "ParseLevelActions.h"
#include "Game/Actions/ActLevel.h"
#include "Parser/Utils/ParseUtils.h"

namespace Parser2::Actions
{
	using namespace Parser;
	using namespace rapidjson;

	std::shared_ptr<Action> parseLevelAddHighScore(const Value& elem)
	{
		return std::make_shared<ActLevelAddHighScore>(
			getStringViewKey(elem, "level"),
			getStringViewKey(elem, "scoreName"),
			getUIntKey(elem, "scoreLevel"),
			getUInt64Key(elem, "score"));
	}

	std::shared_ptr<Action> parseLevelLoadGame(const Value& elem)
	{
		return std::make_shared<ActLevelLoadGame>(
			getStringViewKey(elem, "level"),
			getStringViewKey(elem, "file"));
	}

	std::shared_ptr<Action> parseLevelMakeMove(const Value& elem)
	{
		return std::make_shared<ActLevelMakeMove>(getStringViewKey(elem, "level"));
	}

	std::shared_ptr<Action> parseLevelMoveSelectedTile(const Value& elem)
	{
		return std::make_shared<ActLevelMoveSelectedTile>(
			getStringViewKey(elem, "level"),
			getVector2iKey<PairInt16>(elem, "offset"));
	}

	std::shared_ptr<Action> parseLevelNewGame(const Value& elem)
	{
		return std::make_shared<ActLevelNewGame>(getStringViewKey(elem, "level"));
	}

	std::shared_ptr<Action> parseLevelPause(const Value& elem)
	{
		return std::make_shared<ActLevelPause>(
			getStringViewKey(elem, "level"),
			getBoolKey(elem, "pause", true));
	}

	std::shared_ptr<Action> parseLevelQueueUnit(const Value& elem)
	{
		return std::make_shared<ActLevelQueueUnit>(
			getStringViewKey(elem, "level"),
			getStringViewKey(elem, "unit"));
	}

	std::shared_ptr<Action> parseLevelRotateSelectedUnit(const Value& elem)
	{
		return std::make_shared<ActLevelRotateSelectedUnit>(getStringViewKey(elem, "level"));
	}

	std::shared_ptr<Action> parseLevelSaveGame(const Value& elem)
	{
		return std::make_shared<ActLevelSaveGame>(
			getStringViewKey(elem, "level"),
			getStringViewKey(elem, "file"));
	}

	std::shared_ptr<Action> parseLevelSelectTile(const Value& elem)
	{
		return std::make_shared<ActLevelSelectTile>(
			getStringViewKey(elem, "level"),
			getVector2iKey<PairInt16>(elem, "position"));
	}

	std::shared_ptr<Action> parseLevelSelectUnitQueue(const Value& elem)
	{
		return std::make_shared<ActLevelSelectUnitQueue>(
			getStringViewKey(elem, "level"),
			(int16_t)getIntKey(elem, "queue", -1));
	}

	std::shared_ptr<Action> parseLevelSetShader(const Value& elem)
	{
		return std::make_shared<ActLevelSetShader>(
			getStringViewKey(elem, "level"),
			getStringViewKey(elem, "shader"));
	}
}
