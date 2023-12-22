#pragma once

#include "Resources/ImageContainer.h"
#include "Resources/TexturePack.h"
#include "Utils/small_vector.hpp"
#include <vector>

class ImageContainerTexturePack : public TexturePack
{
protected:
	gch::small_vector<std::shared_ptr<ImageContainer>, 1> imgVec{ 1 };
	sf::Vector2f offset;
	uint32_t textureCount{ 0 };
	std::shared_ptr<Palette> palette;
	bool indexed{ false };

	mutable std::vector<std::pair<sf::Texture, ImageContainer::ImageInfo>> cache;

	bool fetchTexture(uint32_t index) const;

public:
	ImageContainerTexturePack(const std::shared_ptr<ImageContainer>& imgVec_,
		const sf::Vector2f& offset_, const std::shared_ptr<Palette>& palette_, bool isIndexed_);

	ImageContainerTexturePack(const std::vector<std::shared_ptr<ImageContainer>>& imgVec_,
		const sf::Vector2f& offset_, const std::shared_ptr<Palette>& palette_, bool isIndexed_);

	auto& getImageContainers() const noexcept { return imgVec; }

	bool get(uint32_t index, TextureInfo& ti) const override;

	sf::Vector2i getTextureSize(uint32_t index) const override;

	const std::shared_ptr<Palette>& getPalette() const noexcept override { return palette; }

	uint32_t size() const noexcept override { return textureCount; }

	uint32_t getGroupCount() const noexcept override { return (uint32_t)imgVec.size(); }

	uint32_t getDirectionCount(uint32_t groupIdx) const noexcept override;

	std::pair<uint32_t, uint32_t> getDirection(uint32_t frameIdx, AnimationFlags& flags) const noexcept override;

	AnimationInfo getAnimation(int32_t groupIdx, int32_t directionIdx) const override;
};
