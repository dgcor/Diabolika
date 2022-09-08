#pragma once

#include "Game/Action.h"
#include "Game/Formula.h"
#include "Game/Properties/UnitLink.h"
#include "Resources/TexturePack.h"
#include <string>
#include "Utils/PairXY.h"
#include <vector>

class UnitClass
{
private:
	std::shared_ptr<TexturePack> texturePack;
	std::shared_ptr<TexturePack> explosionTexturePack;
	uint32_t directions{ 0 };
	int32_t points{ 0 };

	std::string id;
	std::string type;

	std::string spawns;
	Formula spawnCount;
	sf::IntRect spawnArea;

	std::string links;
	UnitLink linkType{ UnitLink::None };
	Formula linkCount;

	std::vector<std::vector<PairInt16>> explodePatterns;
	Formula explodeUnits;
	bool blastExpands{ false };

	std::shared_ptr<Action> createAction;
	std::shared_ptr<Action> destroyAction;

public:
	UnitClass(const std::shared_ptr<TexturePack>& texturePackVar_,
		const std::shared_ptr<TexturePack>& explosionTexturePackVar_,
		uint32_t directions_);

	auto& getTexturePack() const noexcept { return texturePack; }
	auto& getExplosionTexturePack() const noexcept { return explosionTexturePack; }

	auto& Id() const noexcept { return id; }
	auto& Type() const noexcept { return type; }

	void Id(const std::string_view id_) { id = id_; }
	void Type(const std::string_view type_) { type = type_; }

	auto Directions() const noexcept { return directions; }
	auto Points() const noexcept { return points; }

	auto& Spawns() const noexcept { return spawns; }
	auto SpawnArea() const noexcept { return spawnArea; }

	uint16_t generateSpawnCount() const;

	auto& Links() const noexcept { return links; }
	auto LinkType() const noexcept { return linkType; }

	uint16_t generateLinkCount() const;

	auto& ExplodePatterns() const noexcept { return explodePatterns; }
	bool BlastExpands() const noexcept { return blastExpands; }

	uint16_t generateExplodeUnits() const;

	void Points(int32_t val) noexcept { points = val; }

	void Spawns(const std::string_view val) { spawns = val; }
	void SpawnCount(const std::string_view val) noexcept { spawnCount = val; }
	void SpawnArea(const sf::IntRect& val) noexcept { spawnArea = val; }

	void Links(const std::string_view val) { links = val; }
	void LinkType(UnitLink val) noexcept { linkType = val; }
	void LinkCount(const std::string_view val) noexcept { linkCount = val; }

	void ExplodeUnits(const std::string_view val) noexcept { explodeUnits = val; }
	void BlastExpands(bool val) noexcept { blastExpands = val; }

	void addExplodePattern(const std::vector<PairInt16>& pattern) { explodePatterns.push_back(pattern); }

	auto getCreateAction() const noexcept { return createAction; }
	auto getDestroyAction() const noexcept { return destroyAction; }

	void setCreateAction(const std::shared_ptr<Action>& action) { createAction = action; }
	void setDestroyAction(const std::shared_ptr<Action>& action) { destroyAction = action; }
};
