#pragma once

#include "LevelCell.h"
#include "Utils/PairXY.h"
#include <vector>

class LevelBoard
{
private:
	std::vector<LevelCell> cells;
	PairInt16 size;

	sf::FloatRect padding;
	float tileSize{ 0 };

public:
	auto begin() noexcept { return cells.begin(); }
	auto end() noexcept { return cells.end(); }
	auto begin() const noexcept { return cells.begin(); }
	auto end() const noexcept { return cells.end(); }
	auto cbegin() const noexcept { return cells.cbegin(); }
	auto cend() const noexcept { return cells.cend(); }
	auto rbegin() noexcept { return cells.rbegin(); }
	auto rend() noexcept { return cells.rend(); }
	auto rbegin() const noexcept { return cells.rbegin(); }
	auto rend() const noexcept { return cells.rend(); }
	auto crbegin() const noexcept { return cells.crbegin(); }
	auto crend() const noexcept { return cells.crend(); }

	void Init(PairInt16 size_);

	const auto& Size() const noexcept { return size; }

	auto& Padding() noexcept { return padding; }
	auto& Padding() const noexcept { return padding; }

	auto TileSize() const noexcept { return tileSize; }
	void TileSize(float tileSize_) noexcept { tileSize = tileSize_; }

	sf::Vector2f toDrawCoord(int16_t x, int16_t y) const;
	sf::Vector2f toDrawCoord(PairInt16 boardPos) const;

	const LevelCell& get(int16_t x, int16_t y) const;
	LevelCell& get(int16_t x, int16_t y);
	const LevelCell& get(PairInt16 boardPos) const;
	LevelCell& get(PairInt16 boardPos);

	void set(int16_t x, int16_t y, LevelCell&& cell);

	bool isCoordValid(PairInt16 boardPos) const;
	bool isCoordFree(PairInt16 boardPos) const;

	bool hasUnit(PairInt16 boardPos) const;
	std::shared_ptr<Unit> addUnit(const std::shared_ptr<Unit>& unit, PairInt16 boardPos);
	void deleteUnit(const std::shared_ptr<Unit>& unit);
	void deleteUnit(PairInt16 boardPos);

	const Unit* getUnit(int16_t x, int16_t y) const;
	Unit* getUnit(int16_t x, int16_t y);
	const Unit* getUnit(PairInt16 boardPos) const;
	Unit* getUnit(PairInt16 boardPos);
};
