// dll-user.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>

typedef int (*myFunc_t)();

int _tmain(int argc, _TCHAR* argv[])
{
	HMODULE dll = LoadLibraryA( "C:\\Users\\Jani\\Documents\\Visual Studio 2010\\Projects\\dll-examples\\Debug\\dll-examples.dll" );
	void* proc = GetProcAddress( dll, "myFunc" );
	myFunc_t myFunc = (myFunc_t)proc;
	int res = myFunc();
	printf( "from dll: %d\n", res );
	FreeLibrary( dll );
	return 0;
}

