#include "Level.h"
#include "Game.h"
#include "GameUtils.h"
#include "Utils/Utils.h"

static LevelOptions defaultOptions;

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
		tileSize = tileSize_;
	}
	else if (tilesTexturePack_ != nullptr)
	{
		tileSize = (float)tilesTexturePack_->getWidth(0);
	}
	if (tileSize <= 0.f)
	{
		tileSize = 32.f;
	}

	padding.left = (float)std::clamp(padding_.left, 0, 512);
	padding.top = (float)std::clamp(padding_.top, 0, 512);
	padding.width = (float)std::clamp(padding_.width, 0, 512);
	padding.height = (float)std::clamp(padding_.height, 0, 512);
}

std::shared_ptr<Action> Level::getAction(uint16_t nameHash16) const noexcept
{
	switch (nameHash16)
	{
	case str2int16("click"):
	case str2int16("leftClick"):
		return leftClickAction;
	case str2int16("rightClick"):
		return rightClickAction;
	case str2int16("newRound"):
		return newRoundAction;
	case str2int16("selectedTileChange"):
		return selectedTileChangeAction;
	case str2int16("explosion"):
		return explosionAction;
	case str2int16("gameOver"):
		return gameOverAction;
	default:
		return nullptr;
	}
}

bool Level::setAction(uint16_t nameHash16, const std::shared_ptr<Action>& action) noexcept
{
	switch (nameHash16)
	{
	case str2int16("click"):
	case str2int16("leftClick"):
		leftClickAction = action;
		break;
	case str2int16("rightClick"):
		rightClickAction = action;
		break;
	case str2int16("newRound"):
		newRoundAction = action;
		break;
	case str2int16("selectedTileChange"):
		selectedTileChangeAction = action;
		break;
	case str2int16("explosion"):
		explosionAction = action;
		break;
	case str2int16("gameOver"):
		gameOverAction = action;
		break;
	default:
		return false;
	}
	return true;
}

void Level::draw(const Game& game, sf::RenderTarget& target) const
{
	if (Visible() == false)
	{
		return;
	}

	auto origView = target.getView();

	surface.clear(sf::Color::Transparent);

	for (int16_t y = 0; y < mapSize.y; y++)
	{
		for (int16_t x = 0; x < mapSize.x; x++)
		{
			auto& cell = get(x, y);
			surface.draw(game, cell.tile);
		}
	}
	for (int16_t y = 0; y < mapSize.y; y++)
	{
		for (int16_t x = 0; x < mapSize.x; x++)
		{
			auto& cell = get(x, y);
			if (cell.unit != nullptr)
			{
				surface.draw(game, *cell.unit);
			}
		}
	}
	for (int16_t y = 0; y < mapSize.y; y++)
	{
		for (int16_t x = 0; x < mapSize.x; x++)
		{
			auto& cell = get(x, y);
			surface.draw(game, cell.anim);
		}
	}
	surface.draw(game, selectedTileAnim);

	auto surfaceStates(sf::RenderStates::Default);
	if (gameShader != nullptr)
	{
		auto shader = gameShader->shader.get();
		surfaceStates.shader = shader;
		for (auto uniformHash : gameShader->uniforms)
		{
			switch (uniformHash)
			{
			case str2int16("elapsedTime"):
			{
				shader->setUniform("elapsedTime", game.getTotalElapsedTime().asSeconds());
				break;
			}
			case str2int16("mousePosition"):
			{
				if (hasMouseInside == true)
				{
					shader->setUniform("mousePosition", sf::Glsl::Vec2(
						(game.MousePositionf().x - surface.Position().x) /
						surface.Size().x,
						(game.MousePositionf().y - surface.Position().y) /
						surface.Size().y
					));
				}
				break;
			}
			case str2int16("textureSize"):
			{
				shader->setUniform("textureSize", sf::Glsl::Vec2(
					surface.Size().x,
					surface.Size().y
				));
				break;
			}
			case str2int16("visibleRect"):
			{
				shader->setUniform("visibleRect", sf::Glsl::Vec4(
					surface.visibleRect.left,
					surface.visibleRect.top,
					surface.visibleRect.width,
					surface.visibleRect.height
				));
				break;
			}
			default:
				break;
			}
		}
	}
	surface.draw(target, surfaceStates);

	target.setView(origView);
}

void Level::updateSize(const Game& game)
{
	surface.updateSize(game);
	boardRect.left = surface.Position().x + padding.left;
	boardRect.top = surface.Position().y + padding.top;
}

void Level::Position(const sf::Vector2f& position)
{
	surface.Position(position);
	boardRect.left = surface.Position().x + padding.left;
	boardRect.top = surface.Position().y + padding.top;
};

void Level::onMouseButtonPressed(Game& game)
{
	game.clearMousePressed();
	switch (game.MousePress().button)
	{
	case sf::Mouse::Left:
	{
		if (leftClickAction != nullptr)
		{
			game.Events().addBack(leftClickAction);
		}
	}
	break;
	case sf::Mouse::Right:
	{
		if (rightClickAction != nullptr)
		{
			game.Events().addBack(rightClickAction);
		}
	}
	break;
	default:
		break;
	}
}

void Level::update(Game& game)
{
	if (Visible() == false)
	{
		return;
	}
	if (pause == true)
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

	if (gameState != LevelState::GameOver)
	{
		if (boardRect.contains(game.MousePositionf()) == true)
		{
			hasMouseInside = true;

			if (game.wasMouseMoved() == true)
			{
				updateSelectedTile(game);
			}
			if (sleepTime.timeout == sf::Time::Zero &&
				game.wasMousePressed() == true)
			{
				onMouseButtonPressed(game);
			}
		}
		else
		{
			hasMouseInside = false;
		}
	}

	bool allFinished = true;
	for (int16_t y = 0; y < mapSize.y; y++)
	{
		for (int16_t x = 0; x < mapSize.x; x++)
		{
			auto& cell = get(x, y);
			auto finished = cell.anim.hasPlayOnceAnimationFinished();
			if (finished == false)
			{
				sf::FloatRect levelRect({}, surface.Size());
				sf::FloatRect animRect(cell.anim.DrawPosition(), cell.anim.Size());
				finished = levelRect.intersects(animRect) == false;
			}
			allFinished &= finished;
			if (finished == true)
			{
				cell.anim.Visible(false);
			}
			else
			{
				cell.anim.update(game);
			}
			if (cell.unit != nullptr)
			{
				cell.unit->update(game);
			}
		}
	}
	allAnimationsFinished = allFinished;

	updateSelectedTileAnimation(game);
}

bool Level::getProperty(const std::string_view prop, Variable& var) const
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
		var = Variable(levelId);
		return true;
	case str2int16("name"):
		var = Variable(name);
		return true;
	case str2int16("mapSize"):
	{
		if (props.second == "x")
		{
			var = Variable((int64_t)mapSize.x);
		}
		else
		{
			var = Variable((int64_t)mapSize.y);
		}
		return true;
	}
	case str2int16("demons"):
		var = Variable((int64_t)countUnitsByType("demon"));
		return true;
	case str2int16("gameOver"):
		var = Variable(gameState == LevelState::GameOver);
		return true;
	case str2int16("detonators"):
		var = Variable((int64_t)options.numDetonators);
		return true;
	case str2int16("level"):
		var = Variable((int64_t)level);
		return true;
	case str2int16("score"):
		var = Variable((int64_t)score);
		return true;
	case str2int16("chain"):
		var = Variable((int64_t)chain);
		return true;
	case str2int16("selectedMapPosition"):
	{
		auto props2 = Utils::splitStringIn2(props.second, '.');
		if (props2.first == "x")
		{
			var = Variable((int64_t)selectedMapPos.x);
		}
		else if (props2.first == "y")
		{
			var = Variable((int64_t)selectedMapPos.y);
		}
		else if (props2.first == "position")
		{
			if (props2.second == "x")
			{
				var = Variable((int64_t)get(selectedMapPos).tile.Position().x);
			}
			else
			{
				var = Variable((int64_t)get(selectedMapPos).tile.Position().y);
			}
		}
		else
		{
			return false;
		}
		return true;
	}
	case str2int16("selectedUnitQueue"):
		var = Variable((int64_t)selectedUnitQueueIdx);
		return true;
	case str2int16("units"):
		var = Variable((int64_t)options.numUnits);
		return true;
	case str2int16("hasQueuedUnit"):
	{
		auto idx = Utils::strtou(props.second);
		var = Variable(idx < unitQueues.size() && unitQueues[idx].unit != nullptr);
		return true;
	}
	case str2int16("queuedUnit"):
	{
		auto props2 = Utils::splitStringIn2(props.second, '.');
		auto idx = Utils::strtou(props2.first);
		if (idx < unitQueues.size() && unitQueues[idx].unit != nullptr)
		{
			return unitQueues[idx].unit->getProperty(props2.second, var);
		}
		return false;
	}
	case str2int16("hasHighScore"):
	{
		auto idx = Utils::strtou(props.second);
		var = Variable(idx < highScores.size());
		return true;
	}
	case str2int16("highScore"):
		return getHighScoreProperty(props.second, var);
	default:
		return getUIObjProp(propHash, props.second, var);
	}
	return false;
}

bool Level::getHighScoreProperty(const std::string_view prop, Variable& var) const
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
		idx = (int)(highScores.empty() == false ? 0 : -1);
		break;
	case str2int16("lowest"):
		idx = (int)(highScores.empty() == false ? highScores.size() - 1 : -1);
		break;
	default:
		idx = Utils::strtou(props.first);
		break;
	}
	if (idx >= 0 && (size_t)idx < highScores.size())
	{
		if (props.second == "name")
		{
			var = Variable(highScores[idx].name);
			return true;
		}
		else if (props.second == "level")
		{
			var = Variable((int64_t)highScores[idx].level);
			return true;
		}
		else if (props.second == "score")
		{
			var = Variable((int64_t)highScores[idx].score);
			return true;
		}
	}
	return false;
}

const Queryable* Level::getQueryable(const std::string_view prop) const
{
	if (prop.empty() == true)
	{
		return this;
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
		if (idx < unitQueues.size())
		{
			queryable = &*unitQueues[idx].unit;
		}
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

std::vector<std::variant<const Queryable*, Variable>> Level::getQueryableList(
	const std::string_view prop) const
{
	return {};
}

sf::Vector2f Level::toDrawCoord(int16_t x, int16_t y) const
{
	return { padding.left + (tileSize * x), padding.top + (tileSize * y) };
}

sf::Vector2f Level::toDrawCoord(PairInt16 mapPos) const
{
	return toDrawCoord(mapPos.x, mapPos.y);
}

const Level::LevelCell& Level::get(int16_t x, int16_t y) const
{
	return cells[x + y * mapSize.x];
}
Level::LevelCell& Level::get(int16_t x, int16_t y)
{
	return cells[x + y * mapSize.x];
}
const Level::LevelCell& Level::get(PairInt16 mapPos) const
{
	return cells[mapPos.x + mapPos.y * mapSize.x];
}
Level::LevelCell& Level::get(PairInt16 mapPos)
{
	return cells[mapPos.x + mapPos.y * mapSize.x];
}

void Level::addExplosionAnim(Animation& anim)
{
	addExplosionAnim(anim, explosionTexturePack, 0);
}

void Level::addExplosionAnim(Animation& anim,
	const std::shared_ptr<TexturePack>& texturePack, uint32_t direction)
{
	auto animInfo = texturePack->getAnimation(0, direction);
	animInfo.animType = AnimationType::PlayOnce;
	if (animInfo.refresh == sf::Time::Zero)
	{
		animInfo.refresh = sf::milliseconds(50);
	}
	anim.setAnimation(texturePack, animInfo);
	anim.Visible(true);
}

void Level::setTileAnimationPosition(Animation& anim, PairInt16 mapPos) const
{
	auto animSize = anim.Size();
	auto drawCoord = toDrawCoord(mapPos);
	drawCoord.x = drawCoord.x - animSize.x + tileSize;
	drawCoord.y = drawCoord.y - animSize.y + tileSize;
	anim.Position(drawCoord);
}

void Level::updateSelectedTile(Game& game)
{
	auto boardPos = surface.getPosition(game.MousePositionf());
	boardPos.x -= padding.left;
	boardPos.y -= padding.top;
	PairInt16 newSelectedTile(
		(int16_t)std::floor((boardPos.x / boardRect.width) * mapSize.x),
		(int16_t)std::floor((boardPos.y / boardRect.height) * mapSize.y)
	);
	selectTile(game, newSelectedTile);
}

void Level::updateSelectedTileAnimation(Game& game)
{
	selectedTileAnim.Visible(false);
	if (isMapCoordValid(selectedMapPos) == false)
	{
		return;
	}
	if (options.numUnits > 0 && selectedUnit != nullptr)
	{
		if (selectedTileAnim.holdsNullTexturePack() == false)
		{
			selectedTileAnim.setAnimation(nullptr, 0, 0, true, true);
		}

		auto direction = selectedUnit->Direction();
		std::vector<TextureInfo> vecTex;
		float offsetY = 0.f;
		{
			TextureInfo ti;
			if (selectedUnit->getTexture(direction, ti) == true)
			{
				offsetY = ti.textureRect.height - tileSize;
				vecTex.push_back(ti);
			}
		}

		const auto& exploderPatterns = selectedUnit->Class()->ExplodePatterns();
		if (direction < exploderPatterns.size() &&
			explodePatternTexturePack != nullptr)
		{
			TextureInfo ti;
			if (explodePatternTexturePack->get(0, ti) == true)
			{
				sf::Vector2f offset = ti.offset;
				for (const auto& exploderPattern : exploderPatterns[direction])
				{
					auto patternMapPos = selectedMapPos;
					patternMapPos.x += exploderPattern.x;
					patternMapPos.y += exploderPattern.y;

					if (patternMapPos.x < 0 || patternMapPos.x >= mapSize.x ||
						patternMapPos.y < 0 || patternMapPos.y >= mapSize.y)
					{
						continue;
					}
					ti.offset.x = (exploderPattern.x * tileSize) + offset.x;
					ti.offset.y = (exploderPattern.y * tileSize) + offset.y + offsetY;
					vecTex.push_back(ti);
				}
			}
		}

		if (vecTex.empty() == false)
		{
			selectedTileAnim.setTexture(vecTex, true);
			setTileAnimationPosition(selectedTileAnim, selectedMapPos);
			selectedTileAnim.Visible(true);
		}
	}
	else if (options.numUnits == 0)
	{
		if (explodeCursorTexturePack != nullptr)
		{
			if (selectedTileAnim.getTexturePack() != explodeCursorTexturePack)
			{
				selectedTileAnim.setAnimation(explodeCursorTexturePack, 0, 0, true, true);
			}
			else
			{
				selectedTileAnim.update(game);
			}
			selectedTileAnim.Visible(true);
		}
		setTileAnimationPosition(selectedTileAnim, selectedMapPos);
	}
}

void Level::addUnitClass(const std::string_view key, std::shared_ptr<UnitClass> obj)
{
	unitsClasses.insert(std::make_pair(key, std::move(obj)));
}

const UnitClass* Level::getUnitClass(const std::string_view key) const
{
	auto it = unitsClasses.find(sv2str(key));
	if (it != unitsClasses.end())
	{
		return it->second.get();
	}
	return nullptr;
}

void Level::setOptions(std::vector<LevelOptions>& options_)
{
	allOptions = std::move(options_);
	std::sort(allOptions.begin(), allOptions.end(),
		[](const LevelOptions& a, const LevelOptions& b)
		{
			return a.level < b.level;
		}
	);
}

bool Level::isMapCoordValid(PairInt16 mapPos) const
{
	return (mapPos.x >= 0 &&
		mapPos.x < mapSize.x &&
		mapPos.y >= 0 &&
		mapPos.y < mapSize.y);
}

bool Level::hasUnit(PairInt16 mapPos) const
{
	if (isMapCoordValid(mapPos) == true)
	{
		return get(mapPos).unit != nullptr;
	}
	return false;
}

void Level::queueUnit(const std::string_view unitId)
{
	queueUnit(unitId, sharedUnitsQueue);
}

void Level::addUnit(const UnitClass* class_, PairInt16 mapPos)
{
	if (class_ != nullptr)
	{
		addUnit(makeUnit(class_), mapPos);
	}
}

void Level::addUnit(const std::shared_ptr<Unit>& unit)
{
	if (unit != nullptr)
	{
		addUnit(unit, unit->MapPosition());
	}
}

void Level::addUnit(const std::shared_ptr<Unit>& unit, PairInt16 mapPos)
{
	if (isMapCoordValid(mapPos) == true)
	{
		auto& cell = get(mapPos);
		deleteUnit(cell.unit);
		cell.unit = unit;
		if (unit != nullptr)
		{
			unit->MapPosition(mapPos);
			setTileAnimationPosition(*unit, mapPos);
			incrementUnitCountById(unit->Class()->Id());
			incrementUnitCountByType(unit->Class()->Type());
		}
	}
}

void Level::deleteUnit(const std::shared_ptr<Unit>& unit)
{
	if (unit != nullptr)
	{
		deleteUnit(unit->MapPosition());
	}
}

void Level::deleteUnit(PairInt16 mapPos)
{
	if (isMapCoordValid(mapPos) == true)
	{
		auto& cell = get(mapPos);
		if (cell.unit != nullptr)
		{
			decrementUnitCountById(cell.unit->Class()->Id());
			decrementUnitCountByType(cell.unit->Class()->Type());
		}
		cell.unit.reset();
	}
}

const Unit* Level::getUnit(int16_t x, int16_t y) const
{
	return get(x, y).unit.get();
}

Unit* Level::getUnit(int16_t x, int16_t y)
{
	return get(x, y).unit.get();
}

const Unit* Level::getUnit(PairInt16 mapPos) const
{
	return get(mapPos).unit.get();
}

Unit* Level::getUnit(PairInt16 mapPos)
{
	return get(mapPos).unit.get();
}

uint16_t Level::countUnitsById(const std::string_view id) const
{
	auto it = nunUnitsById.find(sv2str(id));
	if (it != nunUnitsById.end())
	{
		return it->second;
	}
	return 0;
}

uint16_t Level::countUnitsByType(const std::string_view type) const
{
	auto it = numUnitsByType.find(sv2str(type));
	if (it != numUnitsByType.end())
	{
		return it->second;
	}
	return 0;
}

uint16_t Level::maxUnitsById(const std::string_view id) const
{
	auto it = options.maxUnitsById.find(sv2str(id));
	if (it != options.maxUnitsById.end())
	{
		return it->second;
	}
	return 0;
}

uint16_t Level::maxUnitsByType(const std::string_view type) const
{
	auto it = options.maxUnitsByType.find(sv2str(type));
	if (it != options.maxUnitsByType.end())
	{
		return it->second;
	}
	return 0;
}

uint16_t Level::incrementUnitCountById(const std::string_view id)
{
	auto it = nunUnitsById.insert(std::make_pair(id, 1));
	if (it.second == false)
	{
		it.first->second++;
		return it.first->second;
	}
	return 0;
}

uint16_t Level::decrementUnitCountById(const std::string_view id)
{
	auto it = nunUnitsById.insert(std::make_pair(id, 0));
	if (it.second == false &&
		it.first->second > 0)
	{
		it.first->second--;
		return it.first->second;
	}
	return 0;
}

uint16_t Level::incrementUnitCountByType(const std::string_view type)
{
	auto it = numUnitsByType.insert(std::make_pair(type, 1));
	if (it.second == false)
	{
		it.first->second++;
		return it.first->second;
	}
	return 0;
}

uint16_t Level::decrementUnitCountByType(const std::string_view type)
{
	auto it = numUnitsByType.insert(std::make_pair(type, 0));
	if (it.second == false &&
		it.first->second > 0)
	{
		it.first->second--;
		return it.first->second;
	}
	return 0;
}

void Level::newGame(Game& game)
{
	pause = false;
	cells.clear();
	mapSize = { 12, 12 };

	int32_t numTiles = -1;
	if (tilesTexturePack != nullptr)
	{
		numTiles = (int32_t)tilesTexturePack->size();
		numTiles--;
	}

	boardRect.left = surface.Position().x + padding.left;
	boardRect.top = surface.Position().y + padding.top;
	boardRect.width = (float)mapSize.x * tileSize;
	boardRect.height = (float)mapSize.y * tileSize;

	surface.Size(sf::Vector2f(
		boardRect.width + (float)(padding.left + padding.width),
		boardRect.height + (float)(padding.top + padding.height)
	));

	for (int16_t y = 0; y < mapSize.y; y++)
	{
		for (int16_t x = 0; x < mapSize.x; x++)
		{
			LevelCell cell;
			if (numTiles >= 0)
			{
				TextureInfo ti;
				auto randomIdx = Utils::Random::get((uint32_t)numTiles);
				if (tilesTexturePack->get(randomIdx, ti) == true)
				{
					cell.tile.setTexture(ti);
				}
			}
			cell.tile.Position(toDrawCoord(x, y));
			cells.push_back(cell);
		}
	}

	surface.setCenter();
	surface.init(game);
	surface.updateView(game);

	allAnimationsFinished = true;
	gameState = LevelState::AddUnits;
	level = 0;
	score = 0;
	chain = 0;
	nunUnitsById.clear();
	numUnitsByType.clear();
	options = defaultOptions;
	for (auto& unitQueue : unitQueues)
	{
		unitQueue = {};
	}
	selectedUnit.reset();
	selectedUnitQueueIdx = -1;
	sharedUnitsQueue = {};
	levelUnitsQueue = {};

	nextRound(game);
}

void Level::nextRound(Game& game)
{
	level++;

	bool hasOptions = false;
	for (const auto& opt : reverse(allOptions))
	{
		if (level >= opt.level)
		{
			options = opt;
			hasOptions = true;
			break;
		}
	}
	if (hasOptions == false)
	{
		options = defaultOptions;
	}

	updateUnitQueues();
	addLevelUnits();

	if (newRoundAction != nullptr)
	{
		game.Events().addBack(newRoundAction);
	}
}

void Level::processStep(Game& game)
{
	switch (gameState)
	{
	case Level::LevelState::Exploding:
	{
		if (currentExplosions.empty() == false)
		{
			processExplosions(game);
		}
		else
		{
			if (options.numDetonators > 0)
			{
				gameState = LevelState::Detonate;
			}
			else
			{
				gameState = LevelState::CheckStatus;
			}
		}
		break;
	}
	case Level::LevelState::CheckStatus:
	{
		if (countUnitsByType("demon") < options.maxDemons)
		{
			nextRound(game);
			gameState = LevelState::AddUnits;
		}
		else
		{
			gameState = LevelState::GameOver;
			if (gameOverAction != nullptr)
			{
				game.Events().addBack(gameOverAction);
			}
		}
		break;
	}
	case Level::LevelState::GameOver:
	default:
		break;
	}
}

std::shared_ptr<Unit> Level::makeUnit(const UnitClass* class_) const
{
	auto unit = std::make_shared<Unit>(class_);
	return unit;
}

void Level::processExplosions(Game& game)
{
	std::vector<PairInt16> newExplosions;

	for (auto& mapPos : currentExplosions)
	{
		auto& cell = get(mapPos);
		if (cell.unit != nullptr)
		{
			auto unitExplTexPack = cell.unit->Class()->getExplosionTexturePack();
			if (unitExplTexPack == nullptr)
			{
				unitExplTexPack = explosionTexturePack;
			}
			addExplosionAnim(cell.anim, unitExplTexPack, cell.unit->Direction());
			cell.unit->explode(game, *this, newExplosions);
		}
		else
		{
			addExplosionAnim(cell.anim);
		}
		setTileAnimationPosition(cell.anim, mapPos);
	}

	drawExplosions = std::move(currentExplosions);
	currentExplosions = std::move(newExplosions);
	sleepTime.timeout = timeout;
	allAnimationsFinished = false;
	chain++;

	if (explosionAction != nullptr)
	{
		game.Events().addBack(explosionAction);
	}
}

void Level::updateUnitQueues()
{
	for (auto& unitQueue : unitQueues)
	{
		updateUnitQueue(unitQueue);
	}
}

void Level::updateUnitQueue(LevelUnitQueue& unitQueue)
{
	if (unitQueue.unit != nullptr)
	{
		return;
	}
	if (unitQueue.queue.empty() == false)
	{
		unitQueue.unit = unitQueue.queue.front();
		unitQueue.queue.pop();
		return;
	}
	if (sharedUnitsQueue.empty() == false)
	{
		unitQueue.unit = sharedUnitsQueue.front();
		sharedUnitsQueue.pop();
		return;
	}
	if (unitsClasses.empty() == true ||
		options.queueUnits.empty() == true)
	{
		return;
	}
	auto classId = options.queueUnits[Utils::Random::get(options.queueUnits.size() - 1)];
	queueUnit(classId, unitQueue.queue);
	if (unitQueue.queue.empty() == false)
	{
		updateUnitQueue(unitQueue);
		return;
	}
}

void Level::queueUnit(const std::string_view unitId, std::queue<std::shared_ptr<Unit>>& unitQueue)
{
	auto class_ = getUnitClass(unitId);
	if (class_ == nullptr)
	{
		return;
	}
	auto newUnit = makeUnit(class_);
	if (newUnit == nullptr)
	{
		return;
	}

	unitQueue.push(newUnit);

	const auto& link = newUnit->Class()->Links();
	if (link.empty() == false)
	{
		auto linkClass = getUnitClass(link);
		if (linkClass != nullptr && linkClass->Type() == "exploder")
		{
			auto linkCount = newUnit->Class()->generateLinkCount();
			while (linkCount > 0)
			{
				auto newExplLink = makeUnit(linkClass);
				auto linkType = newUnit->Class()->LinkType();
				if (linkType == UnitLink::Unidirectional)
				{
					newUnit->LinkedTo(newExplLink);
					newExplLink->DisableExplosion(true);
					unitQueue.push(std::move(newExplLink));
				}
				else if (linkType == UnitLink::Bidirectional)
				{
					newUnit->LinkedTo(newExplLink);
					newExplLink->LinkedTo(newUnit);
					unitQueue.push(std::move(newExplLink));
				}
				else if (linkType == UnitLink::Unit)
				{
					sharedUnitsQueue.push(std::move(newExplLink));
				}
				linkCount--;
			}
		}
	}
}

void Level::addLevelUnits()
{
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
			levelUnitsQueue.push(makeUnit(unitClass));
			count--;
		}
	}

	while (levelUnitsQueue.empty() == false)
	{
		const auto& id = levelUnitsQueue.front()->Class()->Id();
		auto max = maxUnitsById(id);
		if (max > 0 && countUnitsById(id) >= max)
		{
			levelUnitsQueue.pop();
			continue;
		}
		const auto& type = levelUnitsQueue.front()->Class()->Type();
		max = maxUnitsByType(type);
		if (max > 0 && countUnitsByType(type) >= max)
		{
			levelUnitsQueue.pop();
			continue;
		}
		PairInt16 mapPos(Utils::Random::get(mapSize.x - 1), Utils::Random::get(mapSize.y - 1));
		if (hasUnit(mapPos) == true)
		{
			continue;
		}
		addUnit(levelUnitsQueue.front(), mapPos);
		levelUnitsQueue.pop();
	}
}

void Level::rotateSelectedUnit()
{
	if (selectedUnit != nullptr)
	{
		selectedUnit->rotateDirection();
	}
}

void Level::selectUnitQueue(int16_t queueIdx)
{
	if ((size_t)queueIdx < unitQueues.size())
	{
		if (selectedUnit == nullptr)
		{
			selectedUnit = unitQueues[queueIdx].unit;
			unitQueues[queueIdx].unit.reset();
			selectedUnitQueueIdx = queueIdx;
		}
		else if (selectedUnit != nullptr &&
			selectedUnitQueueIdx == queueIdx)
		{
			unitQueues[queueIdx].unit = selectedUnit;
			selectedUnit.reset();
			selectedUnitQueueIdx = -1;
		}
	}
}

void Level::selectTile(Game& game, PairInt16 mapPos)
{
	if (mapPos.x >= 0 && mapPos.x < mapSize.x &&
		mapPos.y >= 0 && mapPos.y < mapSize.y)
	{
		if (selectedMapPos != mapPos &&
			selectedTileChangeAction != nullptr)
		{
			game.Events().addBack(selectedTileChangeAction);
		}
		selectedMapPos = mapPos;
	}
}

void Level::moveSelectedTile(Game& game, PairInt16 mapPosOffset)
{
	selectTile(game, selectedMapPos + mapPosOffset);
}

void Level::makeMove(Game& game)
{
	if (Visible() == false)
	{
		return;
	}
	switch (gameState)
	{
	case Level::LevelState::AddUnits:
	{
		if (options.numUnits > 0 &&
			isMapCoordValid(selectedMapPos) == true &&
			selectedUnit != nullptr)
		{
			auto& cell = get(selectedMapPos.x, selectedMapPos.y);
			if (cell.unit == nullptr)
			{
				addUnit(selectedUnit, selectedMapPos);
				selectedUnit.reset();
				updateUnitQueue(unitQueues[selectedUnitQueueIdx]);
				selectedUnitQueueIdx = -1;
				options.numUnits--;
				auto createAction = cell.unit->Class()->getCreateAction();
				if (createAction != nullptr)
				{
					game.Events().addBack(createAction);
				}
				if (options.numUnits <= 0)
				{
					gameState = LevelState::Detonate;
				}
			}
		}
		break;
	}
	case Level::LevelState::Detonate:
	{
		if (options.numDetonators > 0)
		{
			chain = 0;
			currentExplosions.push_back(selectedMapPos);
			options.numDetonators--;
			gameState = LevelState::Exploding;
		}
		break;
	}
	default:
		break;
	}
}

void Level::updateScore(const Unit& unit)
{
	score += (uint64_t)scoreFormula.eval(unit, *this);
}

void Level::addHighScore(const LevelScore& levelScore)
{
	highScores.push_back(levelScore);
	updateHighScores();
}

void Level::setHighScores(std::vector<LevelScore>& highScores_)
{
	highScores = std::move(highScores_);
	updateHighScores();
}

void Level::updateHighScores()
{
	std::sort(highScores.begin(), highScores.end(),
		[](const LevelScore& a, const LevelScore& b)
		{
			return a.score > b.score;
		}
	);
	if (highScores.size() > 10)
	{
		highScores.resize(10);
	}
}
