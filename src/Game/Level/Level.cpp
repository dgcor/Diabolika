#include "Level.h"
#include "Game/Game.h"
#include "LevelDraw.h"
#include "LevelSave.h"
#include "LevelUIObject.h"
#include "Utils/Random.h"

void Level::Init(const std::shared_ptr<TexturePack>& tilesTexturePack_,
	const std::shared_ptr<TexturePack>& explodeCursorTexturePack_,
	const std::shared_ptr<TexturePack>& explodePatternTexturePack_,
	const std::shared_ptr<TexturePack>& explosionTexturePack_,
	float tileSize_, const sf::IntRect& padding_)
{
	tilesTexturePack = tilesTexturePack_;
	explodeCursorTexturePack = explodeCursorTexturePack_;
	explodePatternTexturePack = explodePatternTexturePack_;
	explosionTexturePack = explosionTexturePack_;

	if (tileSize_ > 0.f)
	{
		board.TileSize(tileSize_);
	}
	else if (tilesTexturePack_ != nullptr)
	{
		board.TileSize((float)tilesTexturePack_->getTextureSize(0).x);
	}
	if (board.TileSize() <= 0.f)
	{
		board.TileSize(32.f);
	}

	board.Padding().left = (float)std::clamp(padding_.left, 0, 512);
	board.Padding().top = (float)std::clamp(padding_.top, 0, 512);
	board.Padding().width = (float)std::clamp(padding_.width, 0, 512);
	board.Padding().height = (float)std::clamp(padding_.height, 0, 512);
}

std::shared_ptr<Action> Level::getAction(uint16_t nameHash16) const noexcept
{
	return LevelUIObject::getAction(*this, nameHash16);
}

bool Level::setAction(uint16_t nameHash16, const std::shared_ptr<Action>& action) noexcept
{
	return LevelUIObject::setAction(*this, nameHash16, action);
}

void Level::draw(const Game& game, sf::RenderTarget& target) const
{
	LevelDraw::draw(*this, game, target);
}

void Level::updateSize(const Game& game)
{
	surface.updateSize(game);
	boardRect.left = surface.Position().x + board.Padding().left;
	boardRect.top = surface.Position().y + board.Padding().top;
}

void Level::Position(const sf::Vector2f& position)
{
	surface.Position(position);
	boardRect.left = surface.Position().x + board.Padding().left;
	boardRect.top = surface.Position().y + board.Padding().top;
};

void Level::update(Game& game)
{
	if (Visible() == false ||
		pause == true)
	{
		return;
	}

	if (sleepTime.timeout != sf::Time::Zero &&
		sleepTime.updateAndReset(game.getElapsedTime()) == true)
	{
		sleepTime.timeout = sf::Time::Zero;
	}

	if (sleepTime.timeout == sf::Time::Zero &&
		allAnimationsFinished == true)
	{
		processStep(game);
	}

	if (gameState.state != LevelState::GameOver)
	{
		if (boardRect.contains(game.MousePositionf()) == true)
		{
			hasMouseInside = true;

			if (game.wasMouseMoved() == true)
			{
				LevelDraw::updateSelectedTile(*this, game);
			}
			if (sleepTime.timeout == sf::Time::Zero &&
				game.wasMousePressed() == true)
			{
				LevelUIObject::onMouseButtonPressed(*this, game);
			}
		}
		else
		{
			hasMouseInside = false;
		}
	}

	allAnimationsFinished = LevelDraw::updateBoardAnimations(*this, game);

	LevelDraw::updateSelectedTileAnimation(*this, game);
}

bool Level::getProperty(const std::string_view prop, Variable& var) const
{
	return LevelUIObject::getProperty(*this, prop, var);
}

QueryObject Level::getQueryable(const std::string_view prop) const
{
	return LevelUIObject::getQueryable(*this, prop);
}

void Level::addUnitClass(const std::string_view key, std::shared_ptr<UnitClass> obj)
{
	unitsClasses.insert(std::make_pair(key, std::move(obj)));
}

const UnitClass* Level::getUnitClass(const std::string_view key) const
{
	auto it = unitsClasses.find(key);
	if (it != unitsClasses.end())
	{
		return it->second.get();
	}
	return nullptr;
}

void Level::queueUnit(const std::string_view unitId)
{
	unitManager.queueUnit(*this, unitId);
}

void Level::addUnit(const UnitClass* class_, PairInt16 boardPos)
{
	if (class_ != nullptr)
	{
		addUnit(std::make_shared<Unit>(class_), boardPos);
	}
}

void Level::addUnit(const std::shared_ptr<Unit>& unit)
{
	if (unit != nullptr)
	{
		addUnit(unit, unit->BoardPosition());
	}
}

void Level::addUnit(const std::shared_ptr<Unit>& unit, PairInt16 boardPos)
{
	if (board.get(boardPos).isWall == true)
	{
		return;
	}
	auto oldUnit = board.addUnit(unit, boardPos);
	if (oldUnit != nullptr)
	{
		stats.decrementUnitCount(*oldUnit->Class());
	}
	if (unit != nullptr)
	{
		LevelDraw::setTileAnimationPosition(*this, *unit, boardPos);
		stats.incrementUnitCount(*unit->Class());
	}
}

void Level::deleteUnit(const std::shared_ptr<Unit>& unit)
{
	if (unit != nullptr)
	{
		deleteUnit(unit->BoardPosition());
	}
}

void Level::deleteUnit(PairInt16 boardPos)
{
	auto unit = board.getUnit(boardPos);
	if (unit != nullptr)
	{
		stats.decrementUnitCount(*unit->Class());
		board.deleteUnit(boardPos);
	}
}

void Level::addWall(PairInt16 boardPos)
{
	if (board.isCoordFree(boardPos) == false)
	{
		return;
	}

	auto& cell = board.get(boardPos);
	if (cell.isWall == false)
	{
		gameState.walls++;

		currentWalls.push_back(boardPos);

		cell.isWall = true;
		cell.tile.setAnimation(cell.tileGroupIdx, 0);
		cell.tile.Pause(false);
	}
}

void Level::initNewGame(Game& game)
{
	pause = false;
	board.Init({ 12, 12 });

	boardRect.left = surface.Position().x + board.Padding().left;
	boardRect.top = surface.Position().y + board.Padding().top;
	boardRect.width = (float)board.Size().x * board.TileSize();
	boardRect.height = (float)board.Size().y * board.TileSize();

	surface.Size(sf::Vector2f(
		boardRect.width + (float)(board.Padding().left + board.Padding().width),
		boardRect.height + (float)(board.Padding().top + board.Padding().height)
	));

	for (int16_t y = 0; y < board.Size().y; y++)
	{
		for (int16_t x = 0; x < board.Size().x; x++)
		{
			LevelCell cell;

			auto numTiles = (int32_t)tilesTexturePack->size();
			numTiles--;

			int32_t groupIdx = -1;
			auto boardIdx = x + y * board.Size().y;

			if (boardIdx < (int32_t)boardTiles.size())
			{
				groupIdx = boardTiles[boardIdx];
			}
			else if (numTiles >= 0)
			{
				groupIdx = Random::get(numTiles);
			}

			if (groupIdx >= 0)
			{
				cell.tileGroupIdx = groupIdx;
				cell.tile.setAnimation(tilesTexturePack, groupIdx, 0);
				cell.tile.Pause(true);
			}

			cell.tile.Position(board.toDrawCoord(x, y));
			board.set(x, y, std::move(cell));
		}
	}

	surface.setCenter();
	surface.init(game);
	surface.updateView(game);

	allAnimationsFinished = true;
	gameState = {};
	gameState.state = LevelState::AddUnits;
	stats = {};
	options = {};
	unitManager = {};
}

void Level::newGame(Game& game)
{
	initNewGame(game);
	nextRound(game);
}

void Level::loadGame(Game& game, const std::string_view filePath)
{
	initNewGame(game);
	if (LevelSave::load(*this, filePath) == true)
	{
		game.Events().tryAddBack(newRoundAction);
	}
}

void Level::saveGame(const std::string_view filePath) const
{
	LevelSave::save(*this, filePath);
}

void Level::nextRound(Game& game)
{
	gameState.level++;
	options = optionsManager.get(gameState.level);
	gameState.detonators += options.numDetonators;
	gameState.units += options.numUnits;
	gameState.walls += options.walls;
	unitManager.updateUnitQueues(*this);
	updateLevelWalls();
	addLevelUnits();

	game.Events().tryAddBack(newRoundAction);
}

void Level::processStep(Game& game)
{
	switch (gameState.state)
	{
	case LevelState::Exploding:
	{
		if (currentExplosions.empty() == false)
		{
			processExplosions(game);
		}
		else
		{
			if (gameState.detonators > 0)
			{
				gameState.state = LevelState::Detonate;
			}
			else
			{
				gameState.state = LevelState::CheckStatus;
			}
		}
		break;
	}
	case LevelState::CheckStatus:
	{
		if (stats.countUnitsByType("demon") < options.maxDemons)
		{
			nextRound(game);
			gameState.state = LevelState::AddUnits;
		}
		else
		{
			gameState.state = LevelState::GameOver;
			game.Events().tryAddBack(gameOverAction);
		}
		break;
	}
	case LevelState::GameOver:
	default:
		break;
	}
}

void Level::processExplosions(Game& game)
{
	std::vector<PairInt16> newExplosions;

	for (auto& boardPos : currentExplosions)
	{
		auto& cell = board.get(boardPos);
		if (cell.unit != nullptr)
		{
			auto unitExplTexPack = cell.unit->Class()->getExplosionTexturePack();
			if (unitExplTexPack == nullptr)
			{
				unitExplTexPack = explosionTexturePack;
			}
			LevelDraw::addExplosionAnim(cell.anim, unitExplTexPack, cell.unit->Direction());
			cell.unit->explode(game, *this, newExplosions);
		}
		else if (cell.isWall == false)
		{
			LevelDraw::addExplosionAnim(*this, cell.anim);
		}
		LevelDraw::setTileAnimationPosition(*this, cell.anim, boardPos);
	}

	drawExplosions = std::move(currentExplosions);
	currentExplosions = std::move(newExplosions);
	sleepTime.timeout = timeout;
	allAnimationsFinished = false;
	gameState.chain++;

	game.Events().tryAddBack(explosionAction);
}

void Level::updateLevelWalls()
{
	for (auto& boardPos : currentWalls)
	{
		auto& cell = board.get(boardPos);
		if (cell.isWall == true)
		{
			cell.isWall = false;
			cell.tile.setAnimation(cell.tileGroupIdx, 1);
			cell.tile.Pause(false);
		}
	}
	currentWalls.clear();

	for (auto walls = (int)options.walls; walls > 0; walls--)
	{
		PairInt16 boardPos(Random::get(board.Size().x - 1), Random::get(board.Size().y - 1));
		addWall(boardPos);
	}
}

void Level::addLevelUnits()
{
	std::deque<std::shared_ptr<Unit>> levelUnitsQueue;

	for (const auto& obj : options.insertUnits)
	{
		auto count = std::min((int32_t)obj.second.eval(*this), 64);
		if (count <= 0)
		{
			continue;
		}
		auto unitClass = getUnitClass(obj.first);
		if (unitClass == nullptr)
		{
			continue;
		}
		while (count > 0)
		{
			levelUnitsQueue.push_back(std::make_shared<Unit>(unitClass));
			count--;
		}
	}

	while (levelUnitsQueue.empty() == false)
	{
		const auto& id = levelUnitsQueue.front()->Class()->Id();
		auto max = options.getMaxUnitsById(id);
		if (max > 0 && stats.countUnitsById(id) >= max)
		{
			levelUnitsQueue.pop_front();
			continue;
		}
		const auto& type = levelUnitsQueue.front()->Class()->Type();
		max = options.getMaxUnitsByType(type);
		if (max > 0 && stats.countUnitsByType(type) >= max)
		{
			levelUnitsQueue.pop_front();
			continue;
		}
		PairInt16 boardPos(Random::get(board.Size().x - 1), Random::get(board.Size().y - 1));
		if (board.isCoordFree(boardPos) == false)
		{
			continue;
		}
		addUnit(levelUnitsQueue.front(), boardPos);
		levelUnitsQueue.pop_front();
	}
}

void Level::rotateSelectedUnit()
{
	unitManager.rotateSelectedUnit();
}

void Level::selectUnitQueue(int16_t queueIdx)
{
	unitManager.selectUnitQueue(queueIdx);
}

void Level::selectTile(Game& game, PairInt16 boardPos)
{
	if (boardPos.x >= 0 && boardPos.x < board.Size().x &&
		boardPos.y >= 0 && boardPos.y < board.Size().y)
	{
		if (gameState.selectedPosition != boardPos)
		{
			game.Events().tryAddBack(selectedTileChangeAction);
		}
		gameState.selectedPosition = boardPos;
	}
}

void Level::moveSelectedTile(Game& game, PairInt16 boardPosOffset)
{
	selectTile(game, gameState.selectedPosition + boardPosOffset);
}

void Level::makeMove(Game& game)
{
	if (Visible() == false)
	{
		return;
	}
	switch (gameState.state)
	{
	case LevelState::AddUnits:
	{
		if (gameState.units > 0 &&
			board.isCoordValid(gameState.selectedPosition) == true &&
			unitManager.hasSelectedUnit() == true)
		{
			auto& cell = board.get(gameState.selectedPosition);
			if (cell.isWall == false && cell.unit == nullptr)
			{
				addUnit(unitManager.SelectedUnit(), gameState.selectedPosition);
				unitManager.clearSelectedUnit(*this);
				gameState.units--;
				game.Events().tryAddBack(cell.unit->Class()->getCreateAction());
				if (gameState.units <= 0)
				{
					gameState.state = LevelState::Detonate;
				}
			}
		}
		break;
	}
	case LevelState::Detonate:
	{
		if (gameState.detonators > 0)
		{
			gameState.chain = 0;
			currentExplosions.push_back(gameState.selectedPosition);
			gameState.detonators--;
			gameState.state = LevelState::Exploding;
		}
		break;
	}
	default:
		break;
	}
}

void Level::updateScore(const Unit& unit)
{
	gameState.score += (uint64_t)scoreFormula.eval(unit, *this);
}
