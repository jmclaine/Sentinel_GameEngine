#pragma once

#include "Quatf.h"
#include "WFloat.h"

using namespace System;
using namespace Sentinel::Utilities;

namespace Sentinel { namespace Math
{
	public ref class WQuatf
	{
	private:

		Quatf*		mRef;

	public:

		WQuatf();
		WQuatf( float _pitch, float _yaw, float _roll );
		WQuatf( float _x, float _y, float _z, float _w );
		WQuatf( const Quatf% q );
		WQuatf( const WQuatf% q );
		~WQuatf();
		!WQuatf();

		Quatf*		GetRef();

		WFloat^		X();
		WFloat^		Y();
		WFloat^		Z();
		WFloat^		W();

		WQuatf^		operator * ( const WQuatf^ q );
		WQuatf^		Mul( const WQuatf^ q );
		WQuatf^		Mul( float scalar );

		float		Length();
		float		LengthSquared();
		float		LengthManhattan();

		WQuatf^		Normalize();

		WQuatf^		AxisAngle( float _x, float _y, float _z, float _degrees );
		WQuatf^		AxisAngle();

		WQuatf^		Euler( float _pitch, float _yaw, float _roll );
		WQuatf^		Euler();
	};
}}
