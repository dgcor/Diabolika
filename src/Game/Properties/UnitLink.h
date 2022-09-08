#pragma once

#include <cstdint>

enum class UnitLink : uint32_t
{
	None,
	Unidirectional,
	Bidirectional,
	Unit,
	Size
};
