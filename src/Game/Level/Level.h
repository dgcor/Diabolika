#pragma once

#include "Game/ActionQueryable.h"
#include "Game/UIObject.h"
#include "LevelBoard.h"
#include "LevelOptionsManager.h"
#include "LevelScoreManager.h"
#include "LevelStateObject.h"
#include "LevelStats.h"
#include "LevelUnitManager.h"
#include "Resources/TexturePack.h"
#include "SFML/Surface.h"

class Level : public ActionQueryable, public UIObject
{
private:
	friend class LevelDraw;
	friend class LevelSave;
	friend class LevelUIObject;

	LevelBoard board;
	std::vector<uint16_t> boardTiles;

	Animation selectedTileAnim;

	std::shared_ptr<TexturePack> tilesTexturePack;
	std::shared_ptr<TexturePack> explodeCursorTexturePack;
	std::shared_ptr<TexturePack> explodePatternTexturePack;
	std::shared_ptr<TexturePack> explosionTexturePack;

	GameShader* gameShader{ nullptr };

	std::string levelId;
	std::string name;

	Surface surface;
	sf::FloatRect boardRect;
	bool pause{ false };
	bool hasMouseInside{ true };
	bool allAnimationsFinished{ true };

	LevelStateObject gameState;

	Formula scoreFormula;
	LevelStats stats;
	LevelOptions options;

	LevelUnitManager unitManager;

	ElapsedTime sleepTime;
	sf::Time timeout{ sf::milliseconds(1000) };
	std::vector<PairInt16> drawExplosions;
	std::vector<PairInt16> currentExplosions;
	std::vector<PairInt16> currentWalls;

	LevelOptionsManager optionsManager;

	std::shared_ptr<Action> leftClickAction;
	std::shared_ptr<Action> rightClickAction;
	std::shared_ptr<Action> selectedTileChangeAction;
	std::shared_ptr<Action> newRoundAction;
	std::shared_ptr<Action> explosionAction;
	std::shared_ptr<Action> gameOverAction;

	UnorderedStringMap<std::shared_ptr<UnitClass>> unitsClasses;

	LevelScoreManager highScoresManager;

	void initNewGame(Game& game);
	void nextRound(Game& game);
	void processStep(Game& game);
	void processExplosions(Game& game);
	void updateLevelWalls();
	void addLevelUnits();

public:
	void Init(const std::shared_ptr<TexturePack>& tilesTexturePack_,
		const std::shared_ptr<TexturePack>& explodeCursorTexturePack_,
		const std::shared_ptr<TexturePack>& explodePatternTexturePack_,
		const std::shared_ptr<TexturePack>& explosionTexturePack_,
		float tileSize_, const sf::IntRect& padding_);

	void setShader(GameShader* shader) noexcept { gameShader = shader; }

	void Id(const std::string_view id_) { levelId = id_; }
	void Name(const std::string_view name_) { name = name_; }

	std::shared_ptr<Action> getAction(uint16_t nameHash16) const noexcept override;
	bool setAction(uint16_t nameHash16, const std::shared_ptr<Action>& action) noexcept override;

	Anchor getAnchor() const noexcept override { return surface.getAnchor(); }
	void setAnchor(const Anchor anchor) noexcept override { surface.setAnchor(anchor); }

	void updateSize(const Game& game) override;

	const sf::Vector2f& DrawPosition() const noexcept override { return surface.Position(); }
	const sf::Vector2f& Position() const noexcept override { return surface.Position(); }
	void Position(const sf::Vector2f& position) override;
	sf::Vector2f Size() const noexcept override { return surface.Size(); }
	void Size(const sf::Vector2f& size) override {}

	bool Pause() const noexcept { return pause; }
	void Pause(bool pause_) noexcept { pause = pause_; }

	bool Visible() const noexcept override { return surface.visible; }
	void Visible(bool visible) noexcept override { surface.visible = visible; }

	void draw(const Game& game, sf::RenderTarget& target) const override;
	void update(Game& game) override;
	bool getProperty(const std::string_view prop, Variable& var) const override;
	QueryObject getQueryable(const std::string_view prop) const override;

	// game related

	auto& Board() const noexcept { return board; }

	auto& BoardTiles() const noexcept { return boardTiles; }
	void BoardTiles(const std::vector<uint16_t> boardTiles_) noexcept { boardTiles = boardTiles_; }

	auto& Options() const noexcept { return options; }

	auto& AllOptions() noexcept { return optionsManager; }
	auto& AllOptions() const noexcept { return optionsManager; }

	auto& HighScores() noexcept { return highScoresManager; }
	auto& HighScores() const noexcept { return highScoresManager; }

	auto& UnitsClasses() const noexcept { return unitsClasses; }

	void addUnitClass(const std::string_view key, std::shared_ptr<UnitClass> obj);
	const UnitClass* getUnitClass(const std::string_view key) const;

	void setScoreFormula(const std::string_view str) { scoreFormula = str; }
	void setTimeout(sf::Time timeout_) { timeout = timeout_; }

	void queueUnit(const std::string_view unitId);
	void addUnit(const UnitClass* class_, PairInt16 boardPos);
	void addUnit(const std::shared_ptr<Unit>& unit);
	void addUnit(const std::shared_ptr<Unit>& unit, PairInt16 boardPos);
	void deleteUnit(const std::shared_ptr<Unit>& unit);
	void deleteUnit(PairInt16 boardPos);

	void addWall(PairInt16 boardPos);

	void newGame(Game& game);
	void loadGame(Game& game, const std::string_view filePath);
	void saveGame(const std::string_view filePath) const;

	void rotateSelectedUnit();
	void selectUnitQueue(int16_t queueIdx);
	void selectTile(Game& game, PairInt16 boardPos);
	void moveSelectedTile(Game& game, PairInt16 boardPosOffset);
	void makeMove(Game& game);
	void updateScore(const Unit& unit);
};
