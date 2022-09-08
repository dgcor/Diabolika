#pragma once

#include "Game/Action.h"
#include "Json/JsonParser.h"
#include <memory>

namespace Parser2::Actions
{
	std::shared_ptr<Action> parseLevelAddHighScore(const rapidjson::Value& elem);

	std::shared_ptr<Action> parseLevelLoadGame(const rapidjson::Value& elem);

	std::shared_ptr<Action> parseLevelMakeMove(const rapidjson::Value& elem);

	std::shared_ptr<Action> parseLevelMoveSelectedTile(const rapidjson::Value& elem);

	std::shared_ptr<Action> parseLevelNewGame(const rapidjson::Value& elem);

	std::shared_ptr<Action> parseLevelPause(const rapidjson::Value& elem);

	std::shared_ptr<Action> parseLevelQueueUnit(const rapidjson::Value& elem);

	std::shared_ptr<Action> parseLevelRotateSelectedUnit(const rapidjson::Value& elem);

	std::shared_ptr<Action> parseLevelSaveGame(const rapidjson::Value& elem);

	std::shared_ptr<Action> parseLevelSelectTile(const rapidjson::Value& elem);

	std::shared_ptr<Action> parseLevelSelectUnitQueue(const rapidjson::Value& elem);

	std::shared_ptr<Action> parseLevelSetShader(const rapidjson::Value& elem);
}
