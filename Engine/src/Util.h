#pragma once

namespace Sentinel
{
	class InitFailed {};

	#define RV(func) if (!func) return 0;
	#define TV(func) if (!func) throw InitFailed();

	#define UNUSED(var) (void)var;

	#define TRAVERSE_VECTOR(x, vec) for (unsigned int x = 0; x < vec.size(); ++x)
	#define TRAVERSE_LIST(it, obj) for (auto it = obj.begin(); it != obj.end(); ++it)
	#define TRAVERSE_LIST_OFFSET(it, obj, offset) for (auto it = obj.begin()+offset; it != obj.end(); ++it)

	#define COPY_ARRAY(dest, src, size)\
		for (unsigned int x = 0; x < size; ++x)\
			dest[x] = src[x];

	#define OUTPUT_ARRAY(count, var)\
		for (int x = 0; x < count; ++x)\
		{\
			var[x].Output(file, width + 1);\
		}
}
