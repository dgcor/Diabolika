#include "LevelUIObject.h"
#include "Game/Game.h"
#include "Game/Utils/UIObjectUtils.h"
#include "Level.h"
#include "Utils/StringHash.h"
#include "Utils/Utils.h"

std::shared_ptr<Action> LevelUIObject::getAction(const Level& level, uint16_t nameHash16) noexcept
{
	switch (nameHash16)
	{
	case str2int16("click"):
	case str2int16("leftClick"):
		return level.leftClickAction;
	case str2int16("rightClick"):
		return level.rightClickAction;
	case str2int16("newRound"):
		return level.newRoundAction;
	case str2int16("selectedTileChange"):
		return level.selectedTileChangeAction;
	case str2int16("explosion"):
		return level.explosionAction;
	case str2int16("gameOver"):
		return level.gameOverAction;
	default:
		return nullptr;
	}
}

bool LevelUIObject::setAction(Level& level, uint16_t nameHash16, const std::shared_ptr<Action>& action) noexcept
{
	switch (nameHash16)
	{
	case str2int16("click"):
	case str2int16("leftClick"):
		level.leftClickAction = action;
		break;
	case str2int16("rightClick"):
		level.rightClickAction = action;
		break;
	case str2int16("newRound"):
		level.newRoundAction = action;
		break;
	case str2int16("selectedTileChange"):
		level.selectedTileChangeAction = action;
		break;
	case str2int16("explosion"):
		level.explosionAction = action;
		break;
	case str2int16("gameOver"):
		level.gameOverAction = action;
		break;
	default:
		return false;
	}
	return true;
}

bool LevelUIObject::getProperty(const Level& level, const std::string_view prop, Variable& var)
{
	if (prop.size() <= 1)
	{
		return false;
	}
	auto props = Utils::splitStringIn2(prop, '.');
	auto propHash = str2int16(props.first);
	switch (propHash)
	{
	case str2int16("id"):
		var = Variable(level.levelId);
		return true;
	case str2int16("name"):
		var = Variable(level.name);
		return true;
	case str2int16("boardSize"):
		var = UIObjectUtils::getTuple2iProp(level.board.Size(), props.second);
		return true;
	case str2int16("demons"):
		var = Variable((int64_t)level.stats.countUnitsByType("demon"));
		return true;
	case str2int16("gameOver"):
		var = Variable(level.gameState.state == LevelState::GameOver);
		return true;
	case str2int16("detonators"):
		var = Variable((int64_t)level.gameState.detonators);
		return true;
	case str2int16("level"):
		var = Variable((int64_t)level.gameState.level);
		return true;
	case str2int16("score"):
		var = Variable((int64_t)level.gameState.score);
		return true;
	case str2int16("chain"):
		var = Variable((int64_t)level.gameState.chain);
		return true;
	case str2int16("selectedPosition"):
	{
		auto props2 = Utils::splitStringIn2(props.second, '.');
		if (props2.first == "position")
		{
			if (level.board.isCoordValid(level.gameState.selectedPosition) == false)
			{
				return false;
			}
			var = UIObjectUtils::getTuple2iProp(level.board.get(level.gameState.selectedPosition).tile.Position(), props2.second);
		}
		else
		{
			var = UIObjectUtils::getTuple2iProp(level.gameState.selectedPosition, props2.first);
		}
		return true;
	}
	case str2int16("selectedUnitQueue"):
		var = Variable((int64_t)level.unitManager.SelectedUnitQueueIdx());
		return true;
	case str2int16("units"):
		var = Variable((int64_t)level.gameState.units);
		return true;
	case str2int16("walls"):
		var = Variable((int64_t)level.gameState.walls);
		return true;
	case str2int16("hasQueuedUnit"):
	{
		auto idx = Utils::strtou(props.second);
		var = Variable(level.unitManager.hasQueuedUnit(idx));
		return true;
	}
	case str2int16("queuedUnit"):
	{
		auto props2 = Utils::splitStringIn2(props.second, '.');
		auto idx = Utils::strtou(props2.first);
		auto unit = level.unitManager.getQueuedUnit(idx);
		if (unit != nullptr)
		{
			return unit->getProperty(props2.second, var);
		}
		return false;
	}
	case str2int16("hasHighScore"):
	{
		auto idx = Utils::strtou(props.second);
		var = Variable(idx < level.highScoresManager.size());
		return true;
	}
	case str2int16("highScore"):
		return level.highScoresManager.getProperty(props.second, var);
	default:
		return level.getUIObjProp(propHash, props.second, var);
	}
	return false;
}

QueryObject LevelUIObject::getQueryable(const Level& level, const std::string_view prop)
{
	if (prop.empty() == true)
	{
		return &level;
	}
	auto props = Utils::splitStringIn2(prop, '.');
	auto propHash = str2int16(props.first);
	const Queryable* queryable = nullptr;
	switch (propHash)
	{
	case str2int16("queue"):
	{
		props = Utils::splitStringIn2(props.second, '.');
		auto idx = Utils::strtou(props.first);
		queryable = level.unitManager.getQueuedUnit(idx);
		break;
	}
	default:
		break;
	}
	if (queryable != nullptr &&
		props.second.empty() == false)
	{
		return queryable->getQueryable(props.second);
	}
	return queryable;
}

void LevelUIObject::onMouseButtonPressed(const Level& level, Game& game)
{
	game.clearMousePressed();
	switch (game.MousePress().button)
	{
	case sf::Mouse::Left:
		game.Events().tryAddBack(level.leftClickAction);
		break;
	case sf::Mouse::Right:
		game.Events().tryAddBack(level.rightClickAction);
		break;
	default:
		break;
	}
}
