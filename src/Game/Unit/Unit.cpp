#include "Unit.h"
#include "Game/Game.h"
#include "Game/Level/Level.h"
#include "Game/Utils/UIObjectUtils.h"
#include "Utils/Random.h"
#include "Utils/StringHash.h"
#include "Utils/Utils.h"

Unit::Unit(const UnitClass* class__, PairInt16 boardPosition_, uint32_t direction_) noexcept
	: class_(class__), boardPosition(boardPosition_), direction(direction_)
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
	case str2int16("boardPosition"):
		var = UIObjectUtils::getTuple2iProp(boardPosition, props.second);
		break;
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
				if (level.Board().isCoordValid(obj->BoardPosition()) == true)
				{
					obj->DisableExplosion(false);
					newExplosions.push_back(obj->BoardPosition());
				}
			}
			break;
		}
		case UnitLink::Bidirectional:
		{
			if (auto obj = linkedTo.lock())
			{
				if (level.Board().isCoordValid(obj->BoardPosition()) == true)
				{
					newExplosions.push_back(obj->BoardPosition());
				}
			}
			break;
		}
		case UnitLink::Unit:
		{
			const auto& link = class_->Links();
			auto boardSize = level.Board().Size();
			PairInt16 expPos;
			for (expPos.y = 0; expPos.y < boardSize.y; expPos.y++)
			{
				for (expPos.x = 0; expPos.x < boardSize.x; expPos.x++)
				{
					auto unit = level.Board().getUnit(expPos);
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
		for (const auto& explodeboardPos : *explodePattern)
		{
			if (explodeboardPos.x == 0 && explodeboardPos.y == 0)
			{
				continue;
			}
			PairInt16 newExplosion(
				boardPosition.x + explodeboardPos.x, boardPosition.y + explodeboardPos.y
			);
			while (true)
			{
				if (level.Board().isCoordValid(newExplosion) == true)
				{
					if (blastExpands == false)
					{
						newExplosions.push_back(newExplosion);
					}
					else
					{
						if (level.Board().hasUnit(newExplosion) == true)
						{
							newExplosions.push_back(newExplosion);
						}
						if (explodeboardPos.x > 0)
						{
							newExplosion.x++;
						}
						else if (explodeboardPos.x < 0)
						{
							newExplosion.x--;
						}
						if (explodeboardPos.y > 0)
						{
							newExplosion.y++;
						}
						else if (explodeboardPos.y < 0)
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
			auto boardSize = level.Board().Size();
			auto maxSize = (uint32_t)boardSize.x * boardSize.y - 1;
			boardSize.x--;
			boardSize.y--;
			if (explodeUnits > maxSize)
			{
				explodeUnits = maxSize;
			}
			std::vector<PairInt16> randomVec;
			while (explodeUnits > 0)
			{
				PairInt16 expPos(Random::get(boardSize.x), Random::get(boardSize.y));
				if (expPos != boardPosition &&
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
				auto boardSize = level.Board().Size();
				boardSize.x--;
				boardSize.y--;

				auto spawnArea = class_->SpawnArea();
				spawnArea.left = std::clamp(boardPosition.x + spawnArea.left, 0, (int)boardSize.x);
				spawnArea.top = std::clamp(boardPosition.y + spawnArea.top, 0, (int)boardSize.x);
				spawnArea.width = std::clamp(boardPosition.x + spawnArea.width, 0, (int)boardSize.x);
				spawnArea.height = std::clamp(boardPosition.y + spawnArea.height, 0, (int)boardSize.y);

				if (spawnArea.left == spawnArea.width &&
					spawnArea.top == spawnArea.height)
				{
					level.addUnit(newClass, boardPosition);
					return;
				}
				else
				{
					level.deleteUnit(boardPosition);
				}

				auto maxTries = 128;
				while (spawnCount > 0 && maxTries > 0)
				{
					PairInt16 pos(
						Random::get(spawnArea.left, spawnArea.width),
						Random::get(spawnArea.top, spawnArea.height)
					);
					if (level.Board().hasUnit(pos) == false)
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

	game.Events().tryAddBack(class_->getDestroyAction());
	level.updateScore(*this);
	level.deleteUnit(boardPosition);
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

void Unit::updateLink(const std::shared_ptr<Unit>& unit, const std::shared_ptr<Unit>& unitToLink)
{
	auto linkType = unit->Class()->LinkType();
	if (linkType == UnitLink::Unidirectional)
	{
		unit->LinkedTo(unitToLink);
		unitToLink->DisableExplosion(true);
	}
	else if (linkType == UnitLink::Bidirectional)
	{
		unit->LinkedTo(unitToLink);
		unitToLink->LinkedTo(unit);
	}
}
