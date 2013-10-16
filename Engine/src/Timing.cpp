#include <time.h>

#include "Timing.h"

namespace Sentinel
{
	const double Timing::INVERSE_FRAME_RATE		= 60.0;
	const double Timing::DESIRED_FRAME_RATE		= 1.0 / 60.0;
	const double Timing::THRESHOLD_FRAME_RATE	= DESIRED_FRAME_RATE * 4.0;

	Timing::Timing()
	{
		mDeltaTime = static_cast< float >(DESIRED_FRAME_RATE);

    #ifdef WIN32
		QueryPerformanceFrequency( &mFrequency );
    #else
        clock_getres( CLOCK_PROCESS_CPUTIME_ID, &m_frequency );
    #endif
	}

	double Timing::QueryCounter()
	{
    #ifdef WIN32
		QueryPerformanceCounter( &mCounter );
		return mCounter.QuadPart / static_cast<double>(mFrequency.QuadPart);
    #else
        clock_gettime( CLOCK_PROCESS_CPUTIME_ID, &m_counter );
        return (double)m_counter.tv_nsec * 0.000000001;
    #endif
	}

	void Timing::Update()
	{
		mTimeNow = QueryCounter();
	}

	void Timing::Limit( double frameRate )
	{
	    double diff = 0;

		mTimeLater = QueryCounter() + diff;
		while( (mTimeLater - mTimeNow) < frameRate )
		{
			mTimeLater = QueryCounter();

			if( mTimeLater < mTimeNow )
			{
			    diff = 1.0 - mTimeNow;
			    mTimeNow = 0;
			}
		}

		mDeltaTime = static_cast< float >(mTimeLater - mTimeNow);
		if( mDeltaTime > (float)THRESHOLD_FRAME_RATE )
			mDeltaTime = (float)THRESHOLD_FRAME_RATE;
	}

	float Timing::DeltaTime()
	{
		return mDeltaTime;
	}
}
