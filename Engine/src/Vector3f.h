#pragma once

#include "Common.h"

namespace Sentinel
{
	class SENTINEL_DLL Vector3f
	{
		friend class Matrix4f;

	public:
		
		float x, y, z;

		////////////////////////////////

		Vector3f();
		Vector3f( float _x, float _y, float _z );
		Vector3f( float* v );

		float		operator [] ( int i ) const;
		float&		operator [] ( int i );

		float*		Ptr() const;
		
		bool		operator == ( const Vector3f& v ) const;
		bool		operator != ( const Vector3f& v ) const;
		bool		Equals( const Vector3f& v ) const;

		Vector3f&	operator += ( const Vector3f& v );
		Vector3f	operator +  ( const Vector3f& v ) const;
		Vector3f	Add( const Vector3f& v ) const;

		Vector3f&	operator -= ( const Vector3f& v );
		Vector3f	operator -  ( const Vector3f& v ) const;
		Vector3f	Sub( const Vector3f& v ) const;

		Vector3f&	operator *= ( float scalar );
		Vector3f	operator *  ( float scalar ) const;
		Vector3f	Mul( float scalar ) const;

		Vector3f&	operator *= ( const Vector3f& v );
		Vector3f	operator *  ( const Vector3f& v ) const;
		Vector3f	Mul( const Vector3f& v ) const;

		float		Length() const;
		float		LengthSquared() const;
		float		LengthManhattan() const;

		Vector3f	Normalize() const;
		Vector3f	NormalizeFast() const;

		float		Dot( const Vector3f& v ) const;
		Vector3f	Cross( const Vector3f& v ) const;

		float		Angle( const Vector3f& v ) const;
	};
}
