#include "UnitClass.h"

UnitClass::UnitClass(const TexturePackVariant& texturePackVar_,
	const TexturePackVariant& explosionTexturePackVar_,
	uint32_t directions_) : texturePackVar(texturePackVar_),
	explosionTexturePackVar(explosionTexturePackVar_), directions(directions_)
{
}

uint16_t UnitClass::generateSpawnCount() const
{
	return std::min((uint16_t)spawnCount.eval(), (uint16_t)64);
}

uint16_t UnitClass::generateLinkCount() const
{
	return std::min((uint16_t)linkCount.eval(), (uint16_t)64);
}

uint16_t UnitClass::generateExplodeUnits() const
{
	return std::min((uint16_t)explodeUnits.eval(), (uint16_t)(12 * 12));
}
