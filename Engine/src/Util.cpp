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

	int StringToID( const char* name )
	{
		int id = 0;
		BYTE value = 0;
		UINT len = strlen( name );

		for( UINT x = 0; x < len && x < 16; ++x )
		{
			if( name[ x ] >= 'A' && name[ x ] <= 'Z' )
				value = name[ x ] - 'A';
			else
			if( name[ x ] >= 'a' && name[ x ] <= 'z' )
				value = name[ x ] - 'a' + 26;
			else
			if( name[ x ] >= '0' && name[ x ] <= '9' )
				value = name[ x ] - '0' + 52;
			else
			if( name[ x ] == '_' )
				value = 63;

			id |= value << (x << 2);
		}

		return id;
	}
}
