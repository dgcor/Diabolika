#pragma once

#include "Game/Variable.h"
#include "LevelUnitQueue.h"
#include <string_view>
#include <vector>

class LevelUnitManager
{
private:
	std::array<LevelUnitQueue, 3> unitQueues;
	std::shared_ptr<Unit> selectedUnit;
	int16_t selectedUnitQueueIdx{ -1 };

	std::deque<std::shared_ptr<Unit>> sharedUnitsQueue;

	void updateUnitQueue(const Level& level, LevelUnitQueue& unitQueue);

	void queueUnit(const Level& level, const std::string_view unitId, std::deque<std::shared_ptr<Unit>>& unitQueue);

public:
	constexpr auto begin() noexcept { return unitQueues.begin(); }
	constexpr auto end() noexcept { return unitQueues.end(); }
	constexpr auto begin() const noexcept { return unitQueues.begin(); }
	constexpr auto end() const noexcept { return unitQueues.end(); }
	constexpr auto cbegin() const noexcept { return unitQueues.cbegin(); }
	constexpr auto cend() const noexcept { return unitQueues.cend(); }
	constexpr auto rbegin() noexcept { return unitQueues.rbegin(); }
	constexpr auto rend() noexcept { return unitQueues.rend(); }
	constexpr auto rbegin() const noexcept { return unitQueues.rbegin(); }
	constexpr auto rend() const noexcept { return unitQueues.rend(); }
	constexpr auto crbegin() const noexcept { return unitQueues.crbegin(); }
	constexpr auto crend() const noexcept { return unitQueues.crend(); }

	bool hasQueuedUnit(size_t queueIndex) const noexcept;

	const Unit* getQueuedUnit(size_t queueIndex) const noexcept;

	// get unit from queu position pair with [queueIndex, unitIndex], where queueIndex starts at 1 and unitIndex at 0
	// the abs(queueIndex) value is used, so -1 is equal to queue 1
	std::shared_ptr<Unit> getQueuedUnit(PairInt16 queuePos) const noexcept;

	// get pair with [queueIndex, unitIndex], where queueIndex starts at 1 and unitIndex at 0
	// returns the queueIndex as a negative value
	PairInt16 getQueuedUnitPosition(const Unit* unit) const noexcept;

	bool hasSelectedUnit() const noexcept { return selectedUnit != nullptr; }

	void clearSelectedUnit(const Level& level) noexcept;

	auto SelectedUnitQueueIdx() const noexcept { return selectedUnitQueueIdx; }

	void selectUnitQueue(int16_t queueIndex);

	auto& SelectedUnit() const noexcept { return selectedUnit; }

	auto& SharedUnitsQueue() const noexcept { return sharedUnitsQueue; }

	void rotateSelectedUnit();

	void updateUnitQueues(const Level& level);

	// creates and queues a new unit and its links to the sharedUnitsQueue
	void queueUnit(const Level& level, const std::string_view unitId);

	// queues a unit to the queue index. queues to the sharedUnitsQueue if the queue index is not valid
	void queueUnit(const std::shared_ptr<Unit>& unit, int16_t queueIndex);
};
