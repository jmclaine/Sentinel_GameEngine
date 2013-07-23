#pragma once

#include "Common.h"

namespace Sentinel
{
	class SENTINEL_DLL Vector2f
	{
	protected:

		float x, y;

	public:

		Vector2f();
		Vector2f( float _x, float _y );
		Vector2f( float* _v );

		float		operator [] ( int i ) const;
		float&		operator [] ( int i );

		float		X() const;
		float		Y() const;
		
		void		SetX( float _x );
		void		SetY( float _y );
		
		float*		Ptr();

		bool		operator == ( const Vector2f& v ) const;
		bool		operator != ( const Vector2f& v ) const;
		bool		Equals( const Vector2f& v ) const;

		Vector2f&	operator += ( const Vector2f& v );
		Vector2f	operator +  ( const Vector2f& v ) const;
		Vector2f	Add( const Vector2f& v ) const;

		Vector2f&	operator -= ( const Vector2f& v );
		Vector2f	operator -  ( const Vector2f& v ) const;
		Vector2f	Sub( const Vector2f& v ) const;

		Vector2f&	operator *= ( float scalar );
		Vector2f	operator *  ( float scalar ) const;
		Vector2f	Mul( float scalar ) const;

		Vector2f&	operator *= ( const Vector2f& v );
		Vector2f	operator *  ( const Vector2f& v ) const;
		Vector2f	Mul( const Vector2f& v ) const;

		float		Length() const;
		float		LengthSquared() const;
		float		LengthManhattan() const;

		Vector2f	Normalize() const;

		float		Dot( const Vector2f& v ) const;
		Vector2f	Cross() const;

		float		Angle( const Vector2f& v ) const;
	};
}