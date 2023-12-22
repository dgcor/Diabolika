#include "LevelBoard.h"

void LevelBoard::Init(PairInt16 size_)
{
	cells.clear();
	cells.resize((int)size_.x * (int)size_.y);
	size = size_;
}

sf::Vector2f LevelBoard::toDrawCoord(int16_t x, int16_t y) const
{
	return { padding.left + (tileSize * x), padding.top + (tileSize * y) };
}

sf::Vector2f LevelBoard::toDrawCoord(PairInt16 boardPos) const
{
	return toDrawCoord(boardPos.x, boardPos.y);
}

const LevelCell& LevelBoard::get(int16_t x, int16_t y) const
{
	return cells[x + y * size.x];
}

LevelCell& LevelBoard::get(int16_t x, int16_t y)
{
	return cells[x + y * size.x];
}

const LevelCell& LevelBoard::get(PairInt16 boardPos) const
{
	return cells[boardPos.x + boardPos.y * size.x];
}

LevelCell& LevelBoard::get(PairInt16 boardPos)
{
	return cells[boardPos.x + boardPos.y * size.x];
}

void LevelBoard::set(int16_t x, int16_t y, LevelCell&& cell)
{
	get(x, y) = std::move(cell);
}

bool LevelBoard::isCoordValid(PairInt16 boardPos) const
{
	return (boardPos.x >= 0 &&
		boardPos.x < size.x &&
		boardPos.y >= 0 &&
		boardPos.y < size.y);
}

bool LevelBoard::isCoordFree(PairInt16 boardPos) const
{
	if (isCoordValid(boardPos) == true)
	{
		const auto& cell = get(boardPos);
		return cell.isWall == false && cell.unit == nullptr;
	}
	return false;
}

bool LevelBoard::hasUnit(PairInt16 boardPos) const
{
	if (isCoordValid(boardPos) == true)
	{
		return get(boardPos).unit != nullptr;
	}
	return false;
}

std::shared_ptr<Unit> LevelBoard::addUnit(const std::shared_ptr<Unit>& unit, PairInt16 boardPos)
{
	if (isCoordValid(boardPos) == true)
	{
		auto& cell = get(boardPos);
		auto oldUnit = cell.unit;
		cell.unit = unit;
		if (unit != nullptr)
		{
			unit->BoardPosition(boardPos);
		}
		return oldUnit;
	}
	return nullptr;
}

void LevelBoard::deleteUnit(const std::shared_ptr<Unit>& unit)
{
	if (unit != nullptr)
	{
		deleteUnit(unit->BoardPosition());
	}
}

void LevelBoard::deleteUnit(PairInt16 boardPos)
{
	if (isCoordValid(boardPos) == true)
	{
		auto& cell = get(boardPos);
		cell.unit.reset();
	}
}

const Unit* LevelBoard::getUnit(int16_t x, int16_t y) const
{
	return get(x, y).unit.get();
}

Unit* LevelBoard::getUnit(int16_t x, int16_t y)
{
	return get(x, y).unit.get();
}

const Unit* LevelBoard::getUnit(PairInt16 boardPos) const
{
	return get(boardPos).unit.get();
}

Unit* LevelBoard::getUnit(PairInt16 boardPos)
{
	return get(boardPos).unit.get();
}
