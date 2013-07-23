#pragma once

#include "Common.h"

namespace Sentinel
{
	class SENTINEL_DLL Vector4f
	{
	protected:

		float x, y, z, w;

	public:

		Vector4f();
		Vector4f( float _x, float _y, float _z, float _w );
		Vector4f( float* _v );

		float		operator [] ( int i ) const;
		float&		operator [] ( int i );

		float		X() const;
		float		Y() const;
		float		Z() const;
		float		W() const;

		void		SetX( float _x );
		void		SetY( float _y );
		void		SetZ( float _z );
		void		SetW( float _w );

		float*		Ptr();

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

		float		Length() const;
		float		LengthSquared() const;
		float		LengthManhattan() const;

		Vector4f	Normalize() const;

		float		Dot( const Vector4f& v ) const;
		Vector4f	Cross( const Vector4f& v ) const;

		float		Angle( const Vector4f& v ) const;
	};
}
