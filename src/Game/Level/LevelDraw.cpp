#include "LevelDraw.h"
#include <cmath>
#include "Game/Game.h"
#include "Level.h"
#include "Utils/StringHash.h"

void LevelDraw::addExplosionAnim(const Level& level, Animation& anim)
{
	addExplosionAnim(anim, level.explosionTexturePack, 0);
}

void LevelDraw::addExplosionAnim(Animation& anim,
	const std::shared_ptr<TexturePack>& texturePack, uint32_t direction)
{
	auto animInfo = texturePack->getAnimation(0, direction);
	animInfo.animType = AnimationType::PlayOnce;
	if (animInfo.refresh == sf::Time::Zero)
	{
		animInfo.refresh = sf::milliseconds(50);
	}
	anim.setAnimation(texturePack, animInfo);
	anim.Visible(true);
}

void LevelDraw::setTileAnimationPosition(const Level& level, Animation& anim, PairInt16 boardPos)
{
	auto animSize = anim.Size();
	auto drawCoord = level.board.toDrawCoord(boardPos);
	drawCoord.x = drawCoord.x - animSize.x + level.board.TileSize();
	drawCoord.y = drawCoord.y - animSize.y + level.board.TileSize();
	anim.Position(drawCoord);
}

bool LevelDraw::updateBoardAnimations(Level& level, Game& game)
{
	bool allFinished = true;
	for (auto& cell : level.board)
	{
		cell.tile.update(game);

		auto finished = cell.anim.hasPlayOnceAnimationFinished();
		if (finished == false)
		{
			sf::FloatRect levelRect({}, level.surface.Size());
			sf::FloatRect animRect(cell.anim.DrawPosition(), cell.anim.Size());
			finished = levelRect.intersects(animRect) == false;
		}
		allFinished &= finished;
		if (finished == true)
		{
			cell.anim.Visible(false);
		}
		else
		{
			cell.anim.update(game);
		}
		if (cell.unit != nullptr)
		{
			cell.unit->update(game);
		}
	}
	return allFinished;
}

void LevelDraw::updateSelectedTile(Level& level, Game& game)
{
	auto boardPos = level.surface.getPosition(game.MousePositionf());
	boardPos.x -= level.board.Padding().left;
	boardPos.y -= level.board.Padding().top;
	PairInt16 newSelectedTile(
		(int16_t)std::floor((boardPos.x / level.boardRect.width) * level.board.Size().x),
		(int16_t)std::floor((boardPos.y / level.boardRect.height) * level.board.Size().y)
	);
	level.selectTile(game, newSelectedTile);
}

void LevelDraw::updateSelectedTileAnimation(Level& level, Game& game)
{
	level.selectedTileAnim.Visible(false);
	if (level.board.isCoordValid(level.gameState.selectedPosition) == false)
	{
		return;
	}
	if (level.gameState.units > 0 && level.unitManager.hasSelectedUnit())
	{
		if (level.selectedTileAnim.holdsNullTexturePack() == false)
		{
			level.selectedTileAnim.setAnimation(nullptr, 0, 0, true, true);
		}

		auto direction = level.unitManager.SelectedUnit()->Direction();
		std::vector<TextureInfo> vecTex;
		float offsetY = 0.f;
		{
			TextureInfo ti;
			if (level.unitManager.SelectedUnit()->getTexture(direction, ti) == true)
			{
				offsetY = ti.textureRect.height - level.board.TileSize();
				vecTex.push_back(ti);
			}
		}

		const auto& exploderPatterns = level.unitManager.SelectedUnit()->Class()->ExplodePatterns();
		if (direction < exploderPatterns.size() &&
			level.explodePatternTexturePack != nullptr)
		{
			TextureInfo ti;
			if (level.explodePatternTexturePack->get(0, ti) == true)
			{
				sf::Vector2f offset = ti.offset;
				for (const auto& exploderPattern : exploderPatterns[direction])
				{
					auto patternboardPos = level.gameState.selectedPosition;
					patternboardPos.x += exploderPattern.x;
					patternboardPos.y += exploderPattern.y;

					if (patternboardPos.x < 0 || patternboardPos.x >= level.board.Size().x ||
						patternboardPos.y < 0 || patternboardPos.y >= level.board.Size().y)
					{
						continue;
					}
					ti.offset.x = (exploderPattern.x * level.board.TileSize()) + offset.x;
					ti.offset.y = (exploderPattern.y * level.board.TileSize()) + offset.y + offsetY;
					vecTex.push_back(ti);
				}
			}
		}

		if (vecTex.empty() == false)
		{
			level.selectedTileAnim.setTexture(vecTex, true);
			setTileAnimationPosition(level, level.selectedTileAnim, level.gameState.selectedPosition);
			level.selectedTileAnim.Visible(true);
		}
	}
	else if (level.gameState.units == 0)
	{
		if (level.explodeCursorTexturePack != nullptr)
		{
			if (level.selectedTileAnim.getTexturePack() != level.explodeCursorTexturePack)
			{
				level.selectedTileAnim.setAnimation(level.explodeCursorTexturePack, 0, 0, true, true);
			}
			else
			{
				level.selectedTileAnim.update(game);
			}
			level.selectedTileAnim.Visible(true);
		}
		setTileAnimationPosition(level, level.selectedTileAnim, level.gameState.selectedPosition);
	}
}

void LevelDraw::draw(const Level& level, const Game& game, sf::RenderTarget& target)
{
	if (level.Visible() == false)
	{
		return;
	}

	auto origView = target.getView();

	level.surface.clear(sf::Color::Transparent);

	for (const auto& cell : level.board)
	{
		level.surface.draw(game, cell.tile);

		if (cell.unit != nullptr)
		{
			level.surface.draw(game, *cell.unit);
		}
	}
	for (const auto& cell : level.board)
	{
		level.surface.draw(game, cell.anim);
	}
	level.surface.draw(game, level.selectedTileAnim);

	auto surfaceStates(sf::RenderStates::Default);
	if (level.gameShader != nullptr)
	{
		auto shader = level.gameShader->shader.get();
		surfaceStates.shader = shader;
		for (auto uniformHash : level.gameShader->uniforms)
		{
			switch (uniformHash)
			{
			case str2int16("elapsedTime"):
			{
				shader->setUniform("elapsedTime", game.getTotalElapsedTime().asSeconds());
				break;
			}
			case str2int16("mousePosition"):
			{
				if (level.hasMouseInside == true)
				{
					shader->setUniform("mousePosition", sf::Glsl::Vec2(
						(game.MousePositionf().x - level.surface.Position().x) /
						level.surface.Size().x,
						(game.MousePositionf().y - level.surface.Position().y) /
						level.surface.Size().y
					));
				}
				break;
			}
			case str2int16("textureSize"):
			{
				shader->setUniform("textureSize", sf::Glsl::Vec2(
					level.surface.Size().x,
					level.surface.Size().y
				));
				break;
			}
			case str2int16("visibleRect"):
			{
				shader->setUniform("visibleRect", sf::Glsl::Vec4(
					level.surface.visibleRect.left,
					level.surface.visibleRect.top,
					level.surface.visibleRect.width,
					level.surface.visibleRect.height
				));
				break;
			}
			default:
				break;
			}
		}
	}
	level.surface.draw(target, surfaceStates);

	target.setView(origView);
}
