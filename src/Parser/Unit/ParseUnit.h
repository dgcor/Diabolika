#pragma once

#include "Json/JsonParser.h"

class Game;

namespace Parser
{
	void parseUnit(Game& game, const rapidjson::Value& elem);
}
