#pragma once

#include "Sentinel.h"
#include "Types.h"

namespace Sentinel
{
	// djb2 algorithm
	// Warning: Collisions may be possible
	//
	// See http://programmers.stackexchange.com/questions/49550/which-hashing-algorithm-is-best-for-uniqueness-and-speed
	// for a complete analysis of more hashing algorithms.
	//
	UINT SENTINEL_DLL HashString(const char* str);
}
