#pragma once

#include "Timing.h"
#include "Debug.h"
#include "Types.h"

namespace Sentinel
{
#define BEGIN_PROFILE(timingName)\
	{\
		static double startCounter = 0;\
		static double timeCounter = 0;\
		static UINT counter = 0;\
		startCounter = timingName->QueryCounter();

#define END_PROFILE(timingName, msg)\
		timeCounter += timingName->QueryCounter() - startCounter;\
		if (++counter >= 60)\
		{\
			Debug::Log(STREAM(msg << ": " << (timeCounter / (double)counter)));\
			counter = 0; timeCounter = 0;\
		}\
	}

#define SEPARATE_PROFILE(msg)\
	{\
		static UINT counter = 0;\
		if (++counter >= 60)\
		{\
			Debug::Log(msg);\
			counter = 0;\
		}\
	}
}
