#pragma once

#include "Game/Drawables/Animation.h"
#include "Game/TextureQueryable.h"
#include "UnitClass.h"

class Level;

class Unit : public Animation, public TextureQueryable
{
private:
	const UnitClass* class_;
	PairInt16 boardPosition;
	uint32_t direction{ 0 };
	int32_t points{ 0 };
	std::weak_ptr<Unit> linkedTo;
	bool disableExplosion{ false };

	void LinkedTo(std::weak_ptr<Unit> linkedTo_) { linkedTo = linkedTo_; }
	void DisableExplosion(bool disableExplosion_) noexcept { disableExplosion = disableExplosion_; }

public:
	Unit(const UnitClass* class__) noexcept : Unit(class__, { -1, -1 }, 0) {}
	Unit(const UnitClass* class__, PairInt16 boardPosition_, uint32_t direction_) noexcept;

	constexpr auto* Class() const noexcept { return class_; }
	auto& BoardPosition() const noexcept { return boardPosition; }
	auto Direction() const noexcept { return direction; }
	auto Points() const noexcept { return points; }
	auto LinkedTo() const noexcept { return linkedTo; }
	bool DisableExplosion() const noexcept { return disableExplosion; }

	void BoardPosition(PairInt16 boardPos) noexcept { boardPosition = boardPos; }
	void Direction(uint32_t direction_) noexcept { direction = direction_; }
	void Points(int32_t points_) noexcept { points = points_; }

	const std::vector<PairInt16>* getExplodePattern() const;

	void rotateDirection();

	void explode(Game& game, Level& level, std::vector<PairInt16>& newExplosions);

	bool getProperty(const std::string_view prop, Variable& var) const override;
	bool getTexture(uint32_t textureNumber, TextureInfo& ti) const override;

	// links 2 units using the first unit's link type
	static void updateLink(const std::shared_ptr<Unit>& unit, const std::shared_ptr<Unit>& unitToLink);
};
