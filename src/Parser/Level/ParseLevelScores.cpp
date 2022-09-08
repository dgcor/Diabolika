#include "ParseLevelScores.h"
#include "Game/Level/LevelScoreManager.h"
#include "Parser/Utils/ParseUtils.h"

namespace Parser
{
	using namespace rapidjson;

	void parseLevelScores(LevelScoreManager& scoreManager, const Value& elem)
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
		scoreManager.set(scores);
	}
}
