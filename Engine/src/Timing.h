#pragma once

#include <time.h>

#ifndef WIN32
    typedef timespec LARGE_INTEGER;
#else
	#include <Windows.h>
#endif

#include "Singleton.h"

namespace Sentinel
{
	class Timing : public Singleton< Timing >
	{
	public:

		static const double DESIRED_FRAME_RATE;
		static const double INVERSE_FRAME_RATE;

	private:

		LARGE_INTEGER	mCounter;
		LARGE_INTEGER	mFrequency;

		double			mTimeNow;
		double			mTimeLater;

		float			mDeltaTime;

	public:

		Timing();
		
		double QueryCounter();
		void   Update();
		void   Limit( double frameRate = DESIRED_FRAME_RATE );
		float  DeltaTime();
	};
}