#pragma once

#include "Json/JsonParser.h"

class Game;

namespace Parser
{
	void parseCursor(Game& game, const rapidjson::Value& elem);
}
