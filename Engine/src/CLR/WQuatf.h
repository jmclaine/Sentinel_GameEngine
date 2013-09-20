#pragma once

#include "Quatf.h"
#include "Property.h"
#include "WVector3f.h"

namespace Sentinel { namespace Math
{
	public ref class WQuatf
	{
		DECLARE_REF( Quatf );
	
	public:

		WQuatf();
		WQuatf( float _pitch, float _yaw, float _roll );
		WQuatf( WVector3f^ rotation );
		WQuatf( float _x, float _y, float _z, float _w );
		WQuatf( const Quatf% q );
		WQuatf( const WQuatf% q );

		////////////////////////////////

		DECLARE_PROPERTY( float, x );
		DECLARE_PROPERTY( float, y );
		DECLARE_PROPERTY( float, z );
		DECLARE_PROPERTY( float, w );

		////////////////////////////////

		DECLARE_OP_DEREF( Quatf );

		static WQuatf^ operator * ( WQuatf^ q0, WQuatf^ q1 );
		WQuatf^		Mul( WQuatf^ q );
		WQuatf^		Mul( float scalar );

		float		Length();
		float		LengthSquared();
		float		LengthManhattan();

		WQuatf^		Normalize();

		WQuatf^		AxisAngle( float _x, float _y, float _z, float _degrees );
		WQuatf^		AxisAngle();

		WQuatf^		Euler( float _pitch, float _yaw, float _roll );
		WQuatf^		Euler();
		WVector3f^	ToEuler();
	};

	DECLARE_CLASS_REF( Quatf );
}}
