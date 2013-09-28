#pragma once

#include <cmath>
#include "Common.h"

namespace Sentinel
{
	#define PI						3.1415926535897932384626433832795
	#define PI_HALF					1.5707963267948966192313216916398
	#define RADIANS_TO_DEGREES		57.295779513082320876798154814105
	#define DEGREES_TO_RADIANS		0.0174532925199432957692369076849
	#define EPSILON					1e-4

	bool SENTINEL_DLL QuadraticFormula( float a, float b, float c, float& r1, float& r2 );

	// Quake III Arena version.
	//
	float SENTINEL_DLL invsqrt( float number );
}
