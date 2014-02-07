#pragma once

#include "Common.h"

namespace Sentinel
{
	class SENTINEL_DLL Vector4f
	{
	public:

		float x, y, z, w;

		////////////////////////////////

		Vector4f();
		Vector4f( float _x, float _y, float _z, float _w );
		Vector4f( float* _v );

		float		operator [] ( int i ) const;
		float&		operator [] ( int i );

		float*		Ptr();

		Vector4f	operator - () const;

		bool		operator == ( const Vector4f& v ) const;
		bool		operator != ( const Vector4f& v ) const;
		bool		Equals( const Vector4f& v ) const;

		Vector4f&	operator += ( const Vector4f& v );
		Vector4f	operator +  ( const Vector4f& v ) const;
		Vector4f	Add( const Vector4f& v ) const;

		Vector4f&	operator -= ( const Vector4f& v );
		Vector4f	operator -  ( const Vector4f& v ) const;
		Vector4f	Sub( const Vector4f& v ) const;

		Vector4f&	operator *= ( float scalar );
		Vector4f	operator *  ( float scalar ) const;
		Vector4f	Mul( float scalar ) const;

		Vector4f&	operator *= ( const Vector4f& v );
		Vector4f	operator *  ( const Vector4f& v ) const;
		Vector4f	Mul( const Vector4f& v ) const;

		Vector4f	Min( const Vector4f& v ) const;
		Vector4f	Max( const Vector4f& v ) const;

		float		Length() const;
		float		LengthSquared() const;
		float		LengthManhattan() const;

		Vector4f	Normalize() const;
		Vector4f	NormalizeFast() const;

		float		Dot( const Vector4f& v ) const;
		Vector4f	Cross( const Vector4f& v ) const;

		float		Angle( const Vector4f& v ) const;

	public:

		static const Vector4f ZERO;
		static const Vector4f ONE;
	};
}
