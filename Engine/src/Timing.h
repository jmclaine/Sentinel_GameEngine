#pragma once

#include <time.h>

#ifndef WIN32
    typedef timespec LARGE_INTEGER;
#else
	#include <Windows.h>
#endif

#include "Common.h"

namespace Sentinel
{
	// Sets a maximum frame rate.
	// Generally, this is 60 Hz as defaulted by DESIRED_FRAME_RATE.
	//
	// Call Update() first to QueryCounter() for the starting time.
	//
	// Call Limit() to temporarily stop the program from executing
	// for a set period of time.
	//
	class SENTINEL_DLL Timing
	{
	public:

		static const double DESIRED_FRAME_RATE;
		static const double INVERSE_FRAME_RATE;
		static const double THRESHOLD_FRAME_RATE;

	private:

		LARGE_INTEGER	mCounter;
		LARGE_INTEGER	mFrequency;

		double			mTimeNow;
		double			mTimeLater;

		float			mDeltaTime;

	public:
		
		Timing();
		
		///////////////////////////////////

		double			QueryCounter();

		void			Update();

		void			Limit( double frameRate = DESIRED_FRAME_RATE );

		float			DeltaTime();
	};
}
