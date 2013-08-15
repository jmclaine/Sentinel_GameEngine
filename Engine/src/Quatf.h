#pragma once

#include "Common.h"
#include "Matrix4f.h"

namespace Sentinel
{
	class SENTINEL_DLL Quatf
	{
		friend class Matrix4f;

	public:

		float x, y, z, w;

		////////////////////////////////

		Quatf();
		Quatf( float _pitch, float _yaw, float _roll );
		Quatf( float _x, float _y, float _z, float _w );

		float*		Ptr();

		bool		operator == ( const Quatf& v ) const;
		bool		operator != ( const Quatf& v ) const;
		bool		Equals( const Quatf& v ) const;

		Quatf		operator * ( const Quatf& q ) const;
		Quatf		Mul( const Quatf& q ) const;
		Quatf		Mul( float scalar ) const;

		float		Length() const;
		float		LengthSquared() const;
		float		LengthManhattan() const;

		Quatf		Normalize() const;

		Quatf&		AxisAngle( float _x, float _y, float _z, float _degrees );
		Quatf&		AxisAngle();

		Quatf&		Euler( float _pitch, float _yaw, float _roll );
		Quatf&		Euler();
	};
}
