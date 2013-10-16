#include "WTiming.h"

namespace Sentinel { namespace Wrapped
{
	DEFINE_REF_PTR( Timing );

	WTiming::WTiming()
	{
		mRef = new Timing();
	}

	WTiming::WTiming( Timing* timing )
	{
		mRef = timing;
	}

	void WTiming::Release()
	{
		delete mRef;
	}
	
	///////////////////////////////////

	double WTiming::QueryCounter()
	{
		return mRef->QueryCounter();
	}

	void WTiming::Update()
	{
		mRef->Update();
	}

	void WTiming::Limit( double frameRate )
	{
		mRef->Limit( frameRate );
	}

	float WTiming::DeltaTime()
	{
		return mRef->DeltaTime();
	}
}}
