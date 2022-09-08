#include "LevelUnitManager.h"
#include <cmath>
#include "Level.h"
#include "Utils/Random.h"

bool LevelUnitManager::hasQueuedUnit(size_t queueIndex) const noexcept
{
	return queueIndex < unitQueues.size() && unitQueues[queueIndex].unit != nullptr;
}

const Unit* LevelUnitManager::getQueuedUnit(size_t queueIndex) const noexcept
{
	if (hasQueuedUnit(queueIndex) == true)
	{
		return unitQueues[queueIndex].unit.get();
	}
	return nullptr;
}

std::shared_ptr<Unit> LevelUnitManager::getQueuedUnit(PairInt16 queuePos) const noexcept
{
	auto queueIndex = queuePos.x;
	if (queueIndex == 0)
	{
		return nullptr;
	}
	queueIndex = (int16_t)(std::abs(queueIndex) - 1);

	if (hasQueuedUnit(queueIndex) == true)
	{
		auto unitIndex = queuePos.y;
		if (unitIndex == 0)
		{
			return unitQueues[queueIndex].unit;
		}
		unitIndex--;
		if ((size_t)unitIndex < unitQueues[queueIndex].queue.size())
		{
			return unitQueues[queueIndex].queue[unitIndex];
		}
	}
	return nullptr;
}

PairInt16 LevelUnitManager::getQueuedUnitPosition(const Unit* unit) const noexcept
{
	if (unit == nullptr)
	{
		return {};
	}
	for (int16_t i = 0; (size_t)i < unitQueues.size(); i++)
	{
		if (unitQueues[i].unit.get() == unit)
		{
			return PairInt16(-i - 1, 0);
		}
		for (int16_t j = 0; (size_t)j < unitQueues[i].queue.size(); j++)
		{
			if (unitQueues[i].queue[j].get() == unit)
			{
				return PairInt16(-i - 1, j + 1);
			}
		}
	}
	return {};
}

void LevelUnitManager::clearSelectedUnit(const Level& level) noexcept
{
	selectedUnit.reset();
	updateUnitQueue(level, unitQueues[selectedUnitQueueIdx]);
	selectedUnitQueueIdx = -1;
}

void LevelUnitManager::selectUnitQueue(int16_t queueIndex)
{
	if ((size_t)queueIndex < unitQueues.size())
	{
		if (selectedUnit == nullptr)
		{
			selectedUnit = unitQueues[queueIndex].unit;
			unitQueues[queueIndex].unit.reset();
			selectedUnitQueueIdx = queueIndex;
		}
		else if (selectedUnit != nullptr &&
			selectedUnitQueueIdx == queueIndex)
		{
			unitQueues[queueIndex].unit = selectedUnit;
			selectedUnit.reset();
			selectedUnitQueueIdx = -1;
		}
	}
}

void LevelUnitManager::rotateSelectedUnit()
{
	if (selectedUnit != nullptr)
	{
		selectedUnit->rotateDirection();
	}
}

void LevelUnitManager::updateUnitQueues(const Level& level)
{
	for (auto& unitQueue : unitQueues)
	{
		updateUnitQueue(level, unitQueue);
	}
}

void LevelUnitManager::updateUnitQueue(const Level& level, LevelUnitQueue& unitQueue)
{
	if (unitQueue.unit != nullptr)
	{
		return;
	}
	if (unitQueue.queue.empty() == false)
	{
		unitQueue.unit = unitQueue.queue.front();
		unitQueue.queue.pop_front();
		return;
	}
	if (sharedUnitsQueue.empty() == false)
	{
		unitQueue.unit = sharedUnitsQueue.front();
		sharedUnitsQueue.pop_front();
		return;
	}
	if (level.UnitsClasses().empty() == true ||
		level.Options().queueUnits.empty() == true)
	{
		return;
	}
	auto classId = level.Options().queueUnits[Random::get(level.Options().queueUnits.size() - 1)];
	queueUnit(level, classId, unitQueue.queue);
	if (unitQueue.queue.empty() == false)
	{
		updateUnitQueue(level, unitQueue);
		return;
	}
}

void LevelUnitManager::queueUnit(const Level& level, const std::string_view unitId)
{
	queueUnit(level, unitId, sharedUnitsQueue);
}

void LevelUnitManager::queueUnit(const Level& level, const std::string_view unitId, std::deque<std::shared_ptr<Unit>>& unitQueue)
{
	auto class_ = level.getUnitClass(unitId);
	if (class_ == nullptr)
	{
		return;
	}
	auto newUnit = std::make_shared<Unit>(class_);
	if (newUnit == nullptr)
	{
		return;
	}

	unitQueue.push_back(newUnit);

	const auto& link = newUnit->Class()->Links();
	if (link.empty() == false)
	{
		auto linkClass = level.getUnitClass(link);
		if (linkClass != nullptr && linkClass->Type() == "exploder")
		{
			auto linkCount = newUnit->Class()->generateLinkCount();
			while (linkCount > 0)
			{
				auto newExplLink = std::make_shared<Unit>(linkClass);

				Unit::updateLink(newUnit, newExplLink);

				if (newUnit->Class()->LinkType() != UnitLink::Unit)
				{
					unitQueue.push_back(std::move(newExplLink));
				}
				else
				{
					sharedUnitsQueue.push_back(std::move(newExplLink));
				}
				linkCount--;
			}
		}
	}
}

void LevelUnitManager::queueUnit(const std::shared_ptr<Unit>& unit, int16_t queueIndex)
{
	if (queueIndex >= 0 && (size_t)queueIndex < unitQueues.size())
	{
		if (unitQueues[queueIndex].unit == nullptr)
		{
			unitQueues[queueIndex].unit = unit;
		}
		else
		{
			unitQueues[queueIndex].queue.push_back(unit);
		}
	}
	else
	{
		sharedUnitsQueue.push_back(unit);
	}
}
