#pragma once

#include "Json/JsonParser.h"

class Game;

namespace Parser
{
	void parseUnitClass(Game& game, const rapidjson::Value& elem);
}
