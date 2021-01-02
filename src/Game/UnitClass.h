#pragma once

#include "Actions/Action.h"
#include "CompositeTexture.h"
#include "Formula.h"
#include "GameProperties.h"
#include <string>
#include "Utils/PairXY.h"
#include <vector>

class UnitClass
{
private:
	TexturePackVariant texturePackVar;
	TexturePackVariant explosionTexturePackVar;
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
	UnitClass(const TexturePackVariant& texturePackVar_,
		const TexturePackVariant& explosionTexturePackVar_,
		uint32_t directions_);

	const TexturePackVariant& getTexturePack() const noexcept { return texturePackVar; }
	const TexturePackVariant& getExplosionTexturePack() const noexcept { return explosionTexturePackVar; }

	const std::string& Id() const noexcept { return id; }
	const std::string& Type() const noexcept { return type; }

	void Id(const std::string_view id_) { id = id_; }
	void Type(const std::string_view type_) { type = type_; }

	uint32_t Directions() const noexcept { return directions; }
	int32_t Points() const noexcept { return points; }

	const std::string& Spawns() const noexcept { return spawns; }
	sf::IntRect SpawnArea() const noexcept { return spawnArea; }

	uint16_t generateSpawnCount() const;

	const std::string& Links() const noexcept { return links; }
	UnitLink LinkType() const noexcept { return linkType; }

	uint16_t generateLinkCount() const;

	const std::vector<std::vector<PairInt16>>& ExplodePatterns() const noexcept { return explodePatterns; }
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

	std::shared_ptr<Action> getCreateAction() const noexcept { return createAction; }
	std::shared_ptr<Action> getDestroyAction() const noexcept { return destroyAction; }

	void setCreateAction(const std::shared_ptr<Action>& action) { createAction = action; }
	void setDestroyAction(const std::shared_ptr<Action>& action) { destroyAction = action; }
};
