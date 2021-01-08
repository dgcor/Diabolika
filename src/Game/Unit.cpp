#include "Unit.h"
#include "Game.h"
#include "Level.h"
#include "Utils/Utils.h"

Unit::Unit(const UnitClass* class__, PairInt16 mapPosition_, uint32_t direction_) noexcept
	: class_(class__), mapPosition(mapPosition_), direction(direction_)
{
	AnimationInfo animInfo;
	const auto& texturePack = class__->getTexturePack();
	animInfo = texturePack->getAnimation(0, direction_);
	if (animInfo.refresh == sf::Time::Zero)
	{
		animInfo.refresh = sf::milliseconds(50);
	}
	setAnimation(texturePack, animInfo);
	points = class_->Points();
}

const std::vector<PairInt16>* Unit::getExplodePattern() const
{
	auto directions = class_->Directions();
	auto numPatterns = class_->ExplodePatterns().size();
	if (directions == 0 && numPatterns == 1)
	{
		return &class_->ExplodePatterns()[0];
	}
	if (direction < directions && direction < numPatterns)
	{
		return &class_->ExplodePatterns()[direction];
	}
	return nullptr;
}

void Unit::rotateDirection()
{
	auto directions = class_->Directions();
	if (directions > 0 &&
		direction < directions - 1)
	{
		direction++;
	}
	else
	{
		direction = 0;
	}
	setAnimation(0, direction, true, true);
}

bool Unit::getProperty(const std::string_view prop, Variable& var) const
{
	if (prop.size() <= 1)
	{
		return false;
	}
	auto props = Utils::splitStringIn2(prop, '.');
	auto propHash = str2int16(props.first);
	switch (propHash)
	{
	case str2int16("direction"):
		var = Variable((int64_t)direction);
		break;
	case str2int16("points"):
		var = Variable((int64_t)points);
		break;
	case str2int16("disableExplosion"):
		var = Variable(disableExplosion);
		break;
	case str2int16("mapPosition"):
	{
		if (props.second == "x")
		{
			var = Variable((int64_t)mapPosition.x);
		}
		else
		{
			var = Variable((int64_t)mapPosition.y);
		}
		break;
	}
	default:
		return Animation::getProperty(prop, var);
	}
	return true;
}

void Unit::explode(Game& game, Level& level, std::vector<PairInt16>& newExplosions)
{
	auto linkType = class_->LinkType();

	if (linkType != UnitLink::None)
	{
		switch (linkType)
		{
		case UnitLink::Unidirectional:
		{
			if (auto obj = linkedTo.lock())
			{
				if (level.isMapCoordValid(obj->MapPosition()) == true)
				{
					obj->DisableExplosion(false);
					newExplosions.push_back(obj->MapPosition());
				}
			}
			break;
		}
		case UnitLink::Bidirectional:
		{
			if (auto obj = linkedTo.lock())
			{
				if (level.isMapCoordValid(obj->MapPosition()) == true)
				{
					newExplosions.push_back(obj->MapPosition());
				}
			}
			break;
		}
		case UnitLink::Unit:
		{
			const auto& link = class_->Links();
			auto mapSize = level.MapSize();
			PairInt16 expPos;
			for (expPos.y = 0; expPos.y < mapSize.y; expPos.y++)
			{
				for (expPos.x = 0; expPos.x < mapSize.x; expPos.x++)
				{
					auto unit = level.getUnit(expPos);
					if (unit != nullptr && unit->Class()->Id() == link)
					{
						newExplosions.push_back(expPos);
					}
				}
			}
			break;
		}
		default:
			break;
		}
	}

	auto explodePattern = getExplodePattern();
	if (explodePattern != nullptr && disableExplosion == false)
	{
		auto blastExpands = class_->BlastExpands();
		for (const auto& explodeMapPos : *explodePattern)
		{
			if (explodeMapPos.x == 0 && explodeMapPos.y == 0)
			{
				continue;
			}
			PairInt16 newExplosion(
				mapPosition.x + explodeMapPos.x, mapPosition.y + explodeMapPos.y
			);
			while (true)
			{
				if (level.isMapCoordValid(newExplosion) == true)
				{
					if (blastExpands == false)
					{
						newExplosions.push_back(newExplosion);
					}
					else
					{
						if (level.hasUnit(newExplosion) == true)
						{
							newExplosions.push_back(newExplosion);
						}
						if (explodeMapPos.x > 0)
						{
							newExplosion.x++;
						}
						else if (explodeMapPos.x < 0)
						{
							newExplosion.x--;
						}
						if (explodeMapPos.y > 0)
						{
							newExplosion.y++;
						}
						else if (explodeMapPos.y < 0)
						{
							newExplosion.y--;
						}
						continue;
					}
				}
				break;
			}
		}
	}
	else
	{
		auto explodeUnits = class_->generateExplodeUnits();
		if (explodeUnits > 0)
		{
			auto mapSize = level.MapSize();
			auto maxSize = (uint32_t)mapSize.x * mapSize.y - 1;
			mapSize.x--;
			mapSize.y--;
			if (explodeUnits > maxSize)
			{
				explodeUnits = maxSize;
			}
			std::vector<PairInt16> randomVec;
			while (explodeUnits > 0)
			{
				PairInt16 expPos(Utils::Random::get(mapSize.x), Utils::Random::get(mapSize.y));
				if (expPos != mapPosition &&
					std::find(randomVec.begin(), randomVec.end(), expPos) == randomVec.end())
				{
					randomVec.push_back(expPos);
					explodeUnits--;
				}
			}
			newExplosions.insert(newExplosions.end(), randomVec.begin(), randomVec.end());
		}
	}

	if (class_->Spawns().empty() == false)
	{
		auto newClass = level.getUnitClass(class_->Spawns());
		if (newClass != nullptr)
		{
			auto spawnCount = class_->generateSpawnCount();
			if (spawnCount > 0)
			{
				auto mapSize = level.MapSize();
				mapSize.x--;
				mapSize.y--;

				auto spawnArea = class_->SpawnArea();
				spawnArea.left = std::clamp(mapPosition.x + spawnArea.left, 0, (int)mapSize.x);
				spawnArea.top = std::clamp(mapPosition.y + spawnArea.top, 0, (int)mapSize.x);
				spawnArea.width = std::clamp(mapPosition.x + spawnArea.width, 0, (int)mapSize.x);
				spawnArea.height = std::clamp(mapPosition.y + spawnArea.height, 0, (int)mapSize.y);

				if (spawnArea.left == spawnArea.width &&
					spawnArea.top == spawnArea.height)
				{
					level.addUnit(newClass, mapPosition);
					return;
				}
				else
				{
					level.deleteUnit(mapPosition);
				}

				auto maxTries = 128;
				while (spawnCount > 0 && maxTries > 0)
				{
					PairInt16 pos(
						Utils::Random::get(spawnArea.left, spawnArea.width),
						Utils::Random::get(spawnArea.top, spawnArea.height)
					);
					if (level.hasUnit(pos) == false)
					{
						level.addUnit(newClass, pos);
						spawnCount--;
						continue;
					}
					maxTries--;
				}
				return;
			}
		}
	}

	auto destroyAction = class_->getDestroyAction();
	if (destroyAction != nullptr)
	{
		game.Events().addBack(destroyAction);
	}
	level.updateScore(*this);
	level.deleteUnit(mapPosition);
}

bool Unit::getTexture(uint32_t textureNumber, TextureInfo& ti) const
{
	const auto& texturePack = class_->getTexturePack();
	auto directions = texturePack->getDirectionCount(0);
	if (textureNumber < directions)
	{
		auto idx = texturePack->getAnimation(0, textureNumber).indexRange.first;
		return texturePack->get(idx, ti);
	}
	return false;
}
