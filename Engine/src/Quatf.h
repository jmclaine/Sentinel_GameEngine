#pragma once

#include "Common.h"
#include "Matrix4f.h"
#include "Vector3f.h"

namespace Sentinel
{
	class SENTINEL_DLL Quatf
	{
		friend class Matrix4f;

	public:

		static const Quatf IDENTITY;

		////////////////////////////////

		float x, y, z, w;

		////////////////////////////////

		Quatf();
		Quatf( float _pitch, float _yaw, float _roll );
		Quatf( const Vector3f& rotation );
		Quatf( float _x, float _y, float _z, float _w );

		float*		Ptr();

		Quatf		operator - () const;

		bool		operator == ( const Quatf& v ) const;
		bool		operator != ( const Quatf& v ) const;
		bool		Equals( const Quatf& v ) const;

		Quatf&		operator *= ( const Quatf& q );
		Quatf		operator * ( const Quatf& q ) const;
		Quatf		Mul( const Quatf& q ) const;
		Quatf		Mul( float scalar ) const;

		float		Length() const;
		float		LengthSquared() const;
		float		LengthManhattan() const;

		Quatf		Normalize() const;
		Quatf		NormalizeFast() const;

		float		Dot( const Quatf& q ) const;

		Quatf		Inverse() const;

		Quatf&		AxisAngle( float _x, float _y, float _z, float _degrees );
		Quatf&		AxisAngle();

		Quatf&		Euler( float _pitch, float _yaw, float _roll );
		Quatf&		Euler();
		Vector3f	ToEuler();

		Vector3f	Transform( const Vector3f& v );

		Quatf		Slerp( const Quatf& q, float t );
	};
}
