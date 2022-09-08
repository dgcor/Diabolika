#include "LevelOptionsManager.h"
#include <algorithm>
#include "Utils/ReverseIterable.h"

LevelOptions LevelOptionsManager::get(uint32_t level) const
{
	for (const auto& opt : reverse(options))
	{
		if (level >= opt.level)
		{
			return opt;
		}
	}
	return {};
}

void LevelOptionsManager::set(std::vector<LevelOptions>& options_)
{
	options = std::move(options_);
	std::sort(options.begin(), options.end(),
		[](const LevelOptions& a, const LevelOptions& b)
		{
			return a.level < b.level;
		}
	);
}
