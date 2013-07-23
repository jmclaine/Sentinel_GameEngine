#pragma once

#include <string>
template class __declspec(dllexport) std::allocator< char >;
template class __declspec(dllexport) std::basic_string< char, std::char_traits< char >, std::allocator< char >>;

#ifdef BUILD_DLL
	#define SENTINEL_DLL	__declspec(dllexport)
#else
	#define SENTINEL_DLL	__declspec(dllimport)
#endif

#define SENTINEL_DLL_C	extern "C" SENTINEL_DLL

typedef unsigned int	UINT;
typedef unsigned char	UCHAR;
