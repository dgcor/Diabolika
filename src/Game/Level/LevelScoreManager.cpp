#include "LevelScoreManager.h"
#include <algorithm>
#include "Utils/StringHash.h"
#include "Utils/Utils.h"

void LevelScoreManager::add(const LevelScore& levelScore)
{
	levelScores.push_back(levelScore);
	updateScores();
}

void LevelScoreManager::set(std::vector<LevelScore>& levelScores_)
{
	levelScores = std::move(levelScores_);
	updateScores();
}

void LevelScoreManager::updateScores()
{
	std::sort(levelScores.begin(), levelScores.end(),
		[](const LevelScore& a, const LevelScore& b)
		{
			return a.score > b.score;
		}
	);
	if (levelScores.size() > 10)
	{
		levelScores.resize(10);
	}
}

bool LevelScoreManager::getProperty(const std::string_view prop, Variable& var) const
{
	if (prop.size() <= 1)
	{
		return false;
	}
	int idx = -1;
	auto props = Utils::splitStringIn2(prop, '.');
	auto propHash = str2int16(props.first);
	switch (propHash)
	{
	case str2int16("highest"):
		idx = (int)(levelScores.empty() == false ? 0 : -1);
		break;
	case str2int16("lowest"):
		idx = (int)(levelScores.empty() == false ? levelScores.size() - 1 : -1);
		break;
	default:
		idx = Utils::strtou(props.first);
		break;
	}
	if (idx >= 0 && (size_t)idx < levelScores.size())
	{
		if (props.second == "name")
		{
			var = Variable(levelScores[idx].name);
			return true;
		}
		else if (props.second == "level")
		{
			var = Variable((int64_t)levelScores[idx].level);
			return true;
		}
		else if (props.second == "score")
		{
			var = Variable((int64_t)levelScores[idx].score);
			return true;
		}
	}
	return false;
}
