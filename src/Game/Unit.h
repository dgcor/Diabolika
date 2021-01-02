#pragma once

#include "Animation.h"
#include "UnitClass.h"

class Level;

class Unit : public Animation
{
private:
	const UnitClass* class_;
	PairInt16 mapPosition;
	uint32_t direction{ 0 };
	int32_t points{ 0 };
	std::weak_ptr<Unit> linkedTo;
	bool disableExplosion{ false };

public:
	Unit(const UnitClass* class__) noexcept : Unit(class__, { -1, -1 }, 0) {}
	Unit(const UnitClass* class__, PairInt16 mapPosition_, uint32_t direction_) noexcept;

	constexpr const UnitClass* Class() const noexcept { return class_; }
	const PairInt16& MapPosition() const noexcept { return mapPosition; }
	uint32_t Direction() const noexcept { return direction; }
	int32_t Points() const noexcept { return points; }
	std::weak_ptr<Unit> LinkedTo() const noexcept { return linkedTo; }
	bool DisableExplosion() const noexcept { return disableExplosion; }

	void MapPosition(PairInt16 mapPos) noexcept { mapPosition = mapPos; }
	void Direction(uint32_t direction_) noexcept { direction = direction_; }
	void Points(int32_t points_) noexcept { points = points_; }
	void LinkedTo(std::weak_ptr<Unit> linkedTo_) { linkedTo = linkedTo_; }
	void DisableExplosion(bool disableExplosion_) noexcept { disableExplosion = disableExplosion_; }

	const std::vector<PairInt16>* getExplodePattern() const;

	void rotateDirection();

	void explode(Game& game, Level& level, std::vector<PairInt16>& newExplosions);

	bool getProperty(const std::string_view prop, Variable& var) const override;
	bool getTexture(uint32_t textureNumber, TextureInfo& ti) const override;
};
