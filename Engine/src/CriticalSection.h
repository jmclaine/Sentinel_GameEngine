#pragma once

#include <windows.h>

namespace Sentinel
{
	class CriticalSection
	{
	private:

		CRITICAL_SECTION	mCS;

	public:

		CriticalSection()
		{
			InitializeCriticalSection( &mCS );
		}

		~CriticalSection()
		{
			DeleteCriticalSection( &mCS );
		}

		void Lock()
		{
			EnterCriticalSection( &mCS );
		}

		void Unlock()
		{
			LeaveCriticalSection( &mCS );
		}
	};
}