#include "FileUtil.h"

#ifdef WIN32
#include <direct.h>
#endif

namespace Sentinel
{
	void SetDirectory(const char* dest)
	{
	#ifdef WIN32
		char path[FILENAME_MAX];
		_getcwd(path, sizeof(path));
		strcat_s(path, "\\");
		strcat_s(path, dest);
		_chdir(path);
	#else
		chdir(dest);
	#endif
	}

	std::string GetDirectory()
	{
	#ifdef WIN32
		char path[FILENAME_MAX];
		_getcwd(path, sizeof(path));
		return path;
	#else
		_ASSERT(0);	// not supported
	#endif
	}
}
