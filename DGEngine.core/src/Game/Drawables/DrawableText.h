#pragma once

#include "Game/Alignment.h"
#include "Game/UIObject.h"
#include <memory>
#include <string>

class DrawableText : public UIObject
{
public:
	virtual std::unique_ptr<DrawableText> clone() const = 0;

	virtual std::string getText() const = 0;
	virtual bool setText(const std::string& utf8Str) = 0;

	virtual sf::FloatRect getLocalBounds() const = 0;
	virtual sf::FloatRect getGlobalBounds() const = 0;

	virtual unsigned getLineCount() const = 0;
	virtual void setColor(const sf::Color& color) = 0;

	virtual void setHorizontalAlign(const HorizontalAlign align) = 0;
	virtual void setVerticalAlign(const VerticalAlign align) = 0;

	virtual void setHorizontalSpaceOffset(int offset) = 0;
	virtual void setVerticalSpaceOffset(int offset) = 0;

	virtual HorizontalAlign getHorizontalAlign() const = 0;
	virtual VerticalAlign getVerticalAlign() const = 0;
};
