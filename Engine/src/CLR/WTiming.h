#pragma once

#include "Property.h"
#include "Timing.h"

namespace Sentinel { namespace Wrapped
{
	public ref class WTiming
	{
		DECLARE_REF_PTR( Timing );

	public:

		WTiming();
		WTiming( Timing* timing );

		void	Release();

		///////////////////////////////////

		static const double INVERSE_FRAME_RATE		= 60.0;
		static const double DESIRED_FRAME_RATE		= 1.0 / 60.0;
		static const double THRESHOLD_FRAME_RATE	= DESIRED_FRAME_RATE * 4.0;
		
		///////////////////////////////////

		double	QueryCounter();

		void	Update();

		void	Limit( double frameRate );

		float	DeltaTime();
	};
}}
