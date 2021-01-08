#pragma once

#include "Animation.h"
#include <array>
#include "Formula.h"
#include "LevelSurface.h"
#include <memory>
#include <queue>
#include "Save/SaveLevel.h"
#include "SFML/Sprite2.h"
#include "SFML/View2.h"
#include "TexturePacks/TexturePack.h"
#include "UIObject.h"
#include "Unit.h"
#include "Utils/PairXY.h"
#include "Utils/UnorderedStringMap.h"
#include <vector>

struct LevelOptions
{
	uint32_t level{ 0 };
	uint16_t numUnits{ 8 };
	uint16_t numDetonators{ 2 };
	uint16_t maxDemons{ 10 };
	UnorderedStringMap<uint16_t> maxUnitsById;
	UnorderedStringMap<uint16_t> maxUnitsByType;
	std::vector<std::pair<std::string, Formula>> insertUnits;
	std::vector<std::string> queueUnits;
};

struct LevelScore
{
	std::string name;
	uint32_t level{ 0 };
	uint64_t score{ 0 };
};

class Level : public UIObject
{
private:
	struct LevelCell
	{
		Image tile;
		Animation anim;
		std::shared_ptr<Unit> unit;
	};

	enum class LevelState
	{
		AddUnits,
		Detonate,
		Exploding,
		CheckStatus,
		GameOver
	};

	struct LevelUnitQueue
	{
		std::shared_ptr<Unit> unit;
		std::queue<std::shared_ptr<Unit>> queue;
	};

	std::vector<LevelCell> cells;
	PairInt16 mapSize;

	Animation selectedTileAnim;

	std::shared_ptr<TexturePack> tilesTexturePack;
	std::shared_ptr<TexturePack> explodeCursorTexturePack;
	std::shared_ptr<TexturePack> explodePatternTexturePack;
	std::shared_ptr<TexturePack> explosionTexturePack;

	float tileSize{ 0 };

	GameShader* gameShader{ nullptr };

	std::string levelId;
	std::string name;

	LevelSurface surface;
	sf::FloatRect padding;
	sf::FloatRect boardRect;
	bool pause{ false };
	bool hasMouseInside{ true };
	bool allAnimationsFinished{ true };
	LevelState gameState{ LevelState::GameOver };

	uint32_t level{ 0 };
	uint64_t score{ 0 };
	uint32_t chain{ 0 };
	Formula scoreFormula;
	UnorderedStringMap<uint16_t> nunUnitsById;
	UnorderedStringMap<uint16_t> numUnitsByType;
	LevelOptions options;

	std::array<LevelUnitQueue, 3> unitQueues;
	std::shared_ptr<Unit> selectedUnit;
	int16_t selectedUnitQueueIdx{ -1 };
	PairInt16 selectedMapPos;

	ElapsedTime sleepTime;
	sf::Time timeout{ sf::milliseconds(1000) };
	std::vector<PairInt16> drawExplosions;
	std::vector<PairInt16> currentExplosions;

	std::vector<LevelOptions> allOptions;

	std::queue<std::shared_ptr<Unit>> sharedUnitsQueue;
	std::queue<std::shared_ptr<Unit>> levelUnitsQueue;

	std::shared_ptr<Action> leftClickAction;
	std::shared_ptr<Action> rightClickAction;
	std::shared_ptr<Action> selectedTileChangeAction;
	std::shared_ptr<Action> newRoundAction;
	std::shared_ptr<Action> explosionAction;
	std::shared_ptr<Action> gameOverAction;

	UnorderedStringMap<std::shared_ptr<UnitClass>> unitsClasses;

	std::vector<LevelScore> highScores;

	void onMouseButtonPressed(Game& game);

	friend void Save::save(const std::string_view filePath,
		const Save::Properties& props, const Game& game, const Level& level);
	friend void Save::serialize(void* serializeObj,
		const Save::Properties& props, const Game& game, const Level& level);

	sf::Vector2f toDrawCoord(int16_t x, int16_t y) const;
	sf::Vector2f toDrawCoord(PairInt16 mapPos) const;

	const LevelCell& get(int16_t x, int16_t y) const;
	LevelCell& get(int16_t x, int16_t y);
	const LevelCell& get(PairInt16 mapPos) const;
	LevelCell& get(PairInt16 mapPos);

	void addExplosionAnim(Animation& anim);
	void addExplosionAnim(Animation& anim,
		const std::shared_ptr<TexturePack>& texturePackVar, uint32_t direction);
	void setTileAnimationPosition(Animation& anim, PairInt16 mapPos) const;
	void updateSelectedTile(Game& game);
	void updateSelectedTileAnimation(Game& game);

	void nextRound(Game& game);
	void processStep(Game& game);
	std::shared_ptr<Unit> makeUnit(const UnitClass* class_) const;
	void processExplosions(Game& game);
	void updateUnitQueues();
	void updateUnitQueue(LevelUnitQueue& unitQueue);
	void queueUnit(const std::string_view unitId, std::queue<std::shared_ptr<Unit>>& unitQueue);
	void addLevelUnits();
	bool getHighScoreProperty(const std::string_view prop, Variable& var) const;
	void updateHighScores();

	uint16_t incrementUnitCountById(const std::string_view id);
	uint16_t decrementUnitCountById(const std::string_view id);
	uint16_t incrementUnitCountByType(const std::string_view type);
	uint16_t decrementUnitCountByType(const std::string_view type);

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

	void save(const std::string_view filePath,
		const Save::Properties& props, const Game& game) const
	{
		Save::save(filePath, props, game, *this);
	}
	void serialize(void* serializeObj, const Save::Properties& props,
		const Game& game, const Level& level)
	{
		Save::serialize(serializeObj, props, game, *this);
	}

	void draw(const Game& game, sf::RenderTarget& target) const override;
	void update(Game& game) override;
	bool getProperty(const std::string_view prop, Variable& var) const override;
	const Queryable* getQueryable(const std::string_view prop) const override;

	std::vector<std::variant<const Queryable*, Variable>> getQueryableList(
		const std::string_view prop) const;

	// game related

	const PairInt16& MapSize() const noexcept { return mapSize; }

	void addUnitClass(const std::string_view key, std::shared_ptr<UnitClass> obj);
	const UnitClass* getUnitClass(const std::string_view key) const;

	void setScoreFormula(const std::string_view str) { scoreFormula = str; }
	void setTimeout(sf::Time timeout_) { timeout = timeout_; }

	void setOptions(std::vector<LevelOptions>& options_);

	bool isMapCoordValid(PairInt16 mapPos) const;

	bool hasUnit(PairInt16 mapPos) const;
	void queueUnit(const std::string_view unitId);
	void addUnit(const UnitClass* class_, PairInt16 mapPos);
	void addUnit(const std::shared_ptr<Unit>& unit);
	void addUnit(const std::shared_ptr<Unit>& unit, PairInt16 mapPos);
	void deleteUnit(const std::shared_ptr<Unit>& unit);
	void deleteUnit(PairInt16 mapPos);

	const Unit* getUnit(int16_t x, int16_t y) const;
	Unit* getUnit(int16_t x, int16_t y);
	const Unit* getUnit(PairInt16 mapPos) const;
	Unit* getUnit(PairInt16 mapPos);

	uint16_t countUnitsById(const std::string_view id) const;
	uint16_t countUnitsByType(const std::string_view type) const;
	uint16_t maxUnitsById(const std::string_view id) const;
	uint16_t maxUnitsByType(const std::string_view type) const;

	void newGame(Game& game);
	void rotateSelectedUnit();
	void selectUnitQueue(int16_t queueIdx);
	void selectTile(Game& game, PairInt16 mapPos);
	void moveSelectedTile(Game& game, PairInt16 mapPosOffset);
	void makeMove(Game& game);
	void updateScore(const Unit& unit);

	void addHighScore(const LevelScore& levelScore);
	void setHighScores(std::vector<LevelScore>& highScores_);
};
