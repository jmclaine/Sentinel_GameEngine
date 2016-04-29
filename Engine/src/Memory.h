#pragma once

#if (_MSC_VER > 1600)
#include <memory>
#endif

namespace Sentinel
{
	#define SAFE_DELETE(var)\
		if (var) { delete (var); (var) = nullptr; }

	#define SAFE_DELETE_ARRAY(var)\
		if (var) { delete[] (var); (var) = nullptr; }

	#define SAFE_DELETE_LIST(var)\
		TRAVERSE_LIST(it, var)\
			SAFE_DELETE(it);\
		var.clear();

	#define SAFE_DELETE_MAP(var)\
		TRAVERSE_LIST(it, var)\
			SAFE_DELETE((it->second));\
		var.clear();

	#define SAFE_RELEASE_DELETE(var)\
		if (var) { var->Release(); delete (var); (var) = nullptr; }

	#define SAFE_RELEASE_PTR(var)\
		if (var) { (var)->Release(); (var) = nullptr; }

	#define SAFE_RELEASE_PTR_LIST(var)\
		TRAVERSE_LIST(it, var)\
			SAFE_RELEASE_PTR((*it));\
		var.clear();

	#define SAFE_RELEASE_LIST(var)\
		TRAVERSE_LIST(it, var)\
			(*it).Release();\
		(*it).clear();

	#define SHUTDOWN_DELETE(var)\
		if (var)\
		{\
			var->Shutdown();\
			delete var;\
			var = nullptr;\
		}

	#define SHUTDOWN_ARRAY(count, var)\
		for (int x = 0; x < count; ++x)\
		{\
			var[x].Shutdown();\
		}\
		if (count > 0) { delete[] var; }
}
