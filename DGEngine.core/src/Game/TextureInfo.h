#pragma once

#include "BlendMode.h"
#include <cstdint>
#include <memory>
#include "Resources/Palette.h"
#include <SFML/Graphics/Texture.hpp>
#include <variant>

struct TextureInfo
{
	const sf::Texture* texture{ nullptr };
	sf::IntRect textureRect;
	std::shared_ptr<Palette> palette;
	sf::Vector2f offset;
	bool absoluteOffset{ false };
	BlendMode blendMode{ BlendMode::Alpha };
	int32_t nextIndex{ -1 };
};

using TextureInfoVar = std::variant<TextureInfo, std::vector<TextureInfo>>;
