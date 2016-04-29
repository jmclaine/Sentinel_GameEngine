#include "Hash.h"

#ifndef USE_DJB2
#include <functional>
#endif

namespace Sentinel
{
	UINT HashString(const std::string& hashString)
	{
	#ifdef USE_DJB2
		// Based on code from: http://www.cse.yorku.ca/~oz/hash.html
		// djb2 algorithm
		// Warning: Collisions may be possible
		
		UINT hash = 5381;
		int c;
		
		for (auto c : hashString)
			hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

		return hash;
	#else
		static auto hash = std::hash<std::string>();

		return hash(hashString);
	#endif
	}
}
