#include "Hash.h"

namespace Sentinel
{
	// Based on code from: http://www.cse.yorku.ca/~oz/hash.html
	// djb2 algorithm
	// Warning: Collisions may be possible
	//
	UINT HashString(const char* str)
	{
		UINT hash = 5381;
		int c;

		while (c = *str++)
			hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

		return hash;
	}
}
