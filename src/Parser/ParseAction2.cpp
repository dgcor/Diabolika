#include "ParseAction2.h"
#include "Actions/ActLevel.h"
#include "Parser/Utils/ParseUtils.h"
#include "Utils/Utils.h"

namespace Parser2
{
	using namespace Parser;
	using namespace rapidjson;

	std::shared_ptr<Action> parseActionElem(Game& game,
		uint16_t nameHash16, const Value& elem)
	{
		switch (nameHash16)
		{
		case str2int16("level.addHighScore"):
		{
			return std::make_shared<ActLevelAddHighScore>(
				getStringViewKey(elem, "level"),
				getStringViewKey(elem, "scoreName"),
				getUIntKey(elem, "scoreLevel"),
				getUInt64Key(elem, "score"));
		}
		case str2int16("level.makeMove"):
		{
			return std::make_shared<ActLevelMakeMove>(getStringViewKey(elem, "level"));
		}
		case str2int16("level.moveSelectedTile"):
		{
			return std::make_shared<ActLevelMoveSelectedTile>(
				getStringViewKey(elem, "level"),
				getVector2iKey<PairInt16>(elem, "offset"));
		}
		case str2int16("level.newGame"):
		{
			return std::make_shared<ActLevelNewGame>(getStringViewKey(elem, "level"));
		}
		case str2int16("level.pause"):
		{
			return std::make_shared<ActLevelPause>(
				getStringViewKey(elem, "level"),
				getBoolKey(elem, "pause", true));
		}
		case str2int16("level.queueUnit"):
		{
			return std::make_shared<ActLevelQueueUnit>(
				getStringViewKey(elem, "level"),
				getStringViewKey(elem, "unit"));
		}
		case str2int16("level.rotateSelectedUnit"):
		{
			return std::make_shared<ActLevelRotateSelectedUnit>(getStringViewKey(elem, "level"));
		}
		case str2int16("level.selectUnitQueue"):
		{
			return std::make_shared<ActLevelSelectUnitQueue>(
				getStringViewKey(elem, "level"),
				(int16_t)getIntKey(elem, "queue", -1));
		}
		case str2int16("level.selectTile"):
		{
			return std::make_shared<ActLevelSelectTile>(
				getStringViewKey(elem, "level"),
				getVector2iKey<PairInt16>(elem, "position"));
		}
		case str2int16("level.setShader"):
		{
			return std::make_shared<ActLevelSetShader>(
				getStringViewKey(elem, "level"),
				getStringViewKey(elem, "shader"));
		}
		default:
			return nullptr;
		}
	}
}
