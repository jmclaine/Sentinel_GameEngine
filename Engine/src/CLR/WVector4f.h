#pragma once

#include "Vector4f.h"

using namespace System;

namespace Sentinel { namespace Math
{
	public ref class WVector4f
	{
	private:

		Vector4f*	mRef;

	public:

		WVector4f();
		WVector4f( float _x, float _y, float _z, float _w );
		WVector4f( const float* v );
		WVector4f( const Vector4f% v );
		WVector4f( const WVector4f% v );
		~WVector4f();

		const Vector4f*	GetRef();

		float		X();
		float		Y();
		float		Z();
		float		W();

		void		SetX( float _x );
		void		SetY( float _y );
		void		SetZ( float _z );
		void		SetW( float _w );

		bool		operator == ( const WVector4f^ v );
		bool		operator != ( const WVector4f^ v );
		bool		Equals( const WVector4f^ v );
		
		WVector4f^	operator += ( const WVector4f^ v );
		WVector4f^	operator +  ( const WVector4f^ v );
		WVector4f^	Add( const WVector4f^ v );

		WVector4f^	operator -= ( const WVector4f^ v );
		WVector4f^	operator -  ( const WVector4f^ v );
		WVector4f^	Sub( const WVector4f^ v );

		WVector4f^	operator *= ( float scalar );
		WVector4f^	operator *  ( float scalar );
		WVector4f^	Mul( float scalar );

		WVector4f^	operator *= ( const WVector4f^ v );
		WVector4f^	operator *  ( const WVector4f^ v );
		WVector4f^	Mul( const WVector4f^ v );

		float		Length();
		float		LengthSquared();
		float		LengthManhattan();

		WVector4f^	Normalize();

		float		Dot( const WVector4f^ v );
		WVector4f^	Cross( const WVector4f^ v );

		float		Angle( const WVector4f^ v );
	};
}}
