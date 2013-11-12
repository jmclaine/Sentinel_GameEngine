#pragma once

#include <string>
template class __declspec(dllexport) std::allocator< char >;
template class __declspec(dllexport) std::basic_string< char, std::char_traits< char >, std::allocator< char >>;

#ifdef BUILD_DLL
	#define SENTINEL_DLL	__declspec(dllexport)
#else
	#ifdef NBUILD_DLL
		#define SENTINEL_DLL
	#else
		#define SENTINEL_DLL	__declspec(dllimport)
	#endif
	#pragma comment( lib, "SentinelEngine.lib" )
#endif

typedef unsigned char	BYTE;
typedef unsigned char	UCHAR;
typedef unsigned int	UINT;
typedef unsigned short	WORD;
typedef unsigned long	DWORD;

// These warnings can be disabled safely.
// They only appear when using STL in a DLL.
//
#pragma warning (disable : 4251)
#pragma warning (disable : 4835)
#pragma warning (disable : 4275)
