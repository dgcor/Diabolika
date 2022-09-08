#pragma once

#include "LevelOptions.h"

class LevelOptionsManager
{
private:
	std::vector<LevelOptions> options;

public:
	auto size() const noexcept { return options.size(); }

	LevelOptions get(uint32_t level) const;

	void set(std::vector<LevelOptions>& options_);
};
