#ifdef WIN32
	#include <direct.h>
#endif

#include "Util.h"

namespace Sentinel
{
	void SetDirectory( const char* dest )
	{
	#ifdef WIN32
		char path[ FILENAME_MAX ];
		_getcwd( path, sizeof( path ));
		strcat_s( path, "\\" );
		strcat_s( path, dest );
		_chdir( path );
	#else
		chdir( dest );
	#endif
	}

	std::string GetDirectory()
	{
	#ifdef WIN32
		char path[ FILENAME_MAX ];
		_getcwd( path, sizeof( path ));
		return path;
	#else
		_ASSERT(0);	// not supported
	#endif
	}

	// Based on code from: http://www.cse.yorku.ca/~oz/hash.html
	// djb2 algorithm
	// Warning: Collisions may be possible
	//
	UINT HashString( const char* str )
	{
		UINT hash = 5381;
		int c;

		while( c = *str++ )
			hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

		return hash;
	}
}
