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
}
