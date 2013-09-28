#include "MathCommon.h"

namespace Sentinel
{
	bool QuadraticFormula( float a, float b, float c, float& r1, float& r2 )
	{
		float q = b*b - 4.0f*a*c;

		if( q >= 0.0f )
		{
			float sq = sqrt(q);
			float d = 1.0f / (2.0f*a);
			r1 = (-b + sq) * d;
			r2 = (-b - sq) * d;

			return true;
		}
		
		return false;
	}

	// Quake III Arena version.
	//
	float invsqrt( float number )
	{
		long i;
		float x2, y;
		const float threehalfs = 1.5F;
 
		x2 = number * 0.5F;
		y  = number;
		i  = * ( long * ) &y;                       // evil floating point bit level hacking
		i  = 0x5f3759df - ( i >> 1 );               // what the fuck?
		y  = * ( float * ) &i;
		y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
	//	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed
 
		return y;
	}
}
