#pragma once

#include <cmath>
#include "Sentinel.h"

namespace Sentinel
{
	#define PI						3.1415926535897932384626433832795
	#define PI_HALF					1.5707963267948966192313216916398
	#define RADIANS_TO_DEGREES		57.295779513082320876798154814105
	#define DEGREES_TO_RADIANS		0.0174532925199432957692369076849
	#define EPSILON					1e-4

	#define INTERP(start, end, alpha) ((start) + ((end) - (start)) * (alpha))

	bool SENTINEL_DLL QuadraticFormula(float a, float b, float c, float& r1, float& r2);

	// Quake III Arena version.
	//
	float SENTINEL_DLL invsqrt(float number);

	#define CLAMP(value, minValue, maxValue)\
		(value < minValue ? minValue : value > maxValue ? maxValue : value)

	#define WRAP(value, minValue, maxValue)\
		(value < minValue ? maxValue : value > maxValue ? minValue : value)

	#define WRAP_NP(value, minValue, maxValue)\
		(value < minValue ? value + maxValue : value > maxValue ? value + minValue : value)

	#define LERP(start, end, alpha)\
		(start + (end - start) * alpha)

	inline bool IsPowerOf2(UINT x)
	{
		return (x != 0) && ((x & (x - 1)) == 0);
	}

	inline int FastMod2(int num)
	{
		return num & 1;
	}

	inline UINT NextPowerOf2(UINT n)
	{
		--n;

		n = n | (n >> 1);
		n = n | (n >> 2);
		n = n | (n >> 4);
		n = n | (n >> 8);
		n = n | (n >> 16);

		return n + 1;
	}
}
