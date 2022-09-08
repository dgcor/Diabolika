#pragma once

#include <memory>
#include <SFML/Graphics/RenderTarget.hpp>
#include "Utils/PairXY.h"

class Animation;
class Game;
class Level;
class TexturePack;

class LevelDraw
{
public:
	static void addExplosionAnim(const Level& level, Animation& anim);

	static void addExplosionAnim(Animation& anim,
		const std::shared_ptr<TexturePack>& texturePack, uint32_t direction);

	static void setTileAnimationPosition(const Level& level, Animation& anim, PairInt16 boardPos);

	static bool updateBoardAnimations(Level& level, Game& game);

	static void updateSelectedTile(Level& level, Game& game);

	static void updateSelectedTileAnimation(Level& level, Game& game);

	static void draw(const Level& level, const Game& game, sf::RenderTarget& target);
};
