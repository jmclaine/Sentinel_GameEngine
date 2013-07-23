#pragma once

#include "Vector4f.h"

using namespace System;

namespace Sentinel { namespace Math
{
	public ref class Vector4fw
	{
	private:

		Vector4f*	mRef;

	public:

		Vector4fw();
		Vector4fw( float _x, float _y, float _z, float _w );
		Vector4fw( const float* v );
		Vector4fw( const Vector4f% v );
		Vector4fw( const Vector4fw% v );
		~Vector4fw();

		const Vector4f*	GetRef();

		float		X();
		float		Y();
		float		Z();
		float		W();

		void		SetX( float _x );
		void		SetY( float _y );
		void		SetZ( float _z );
		void		SetW( float _w );

		bool		operator == ( const Vector4fw^ v );
		bool		operator != ( const Vector4fw^ v );
		bool		Equals( const Vector4fw^ v );
		
		Vector4fw^	operator += ( const Vector4fw^ v );
		Vector4fw^	operator +  ( const Vector4fw^ v );
		Vector4fw^	Add( const Vector4fw^ v );

		Vector4fw^	operator -= ( const Vector4fw^ v );
		Vector4fw^	operator -  ( const Vector4fw^ v );
		Vector4fw^	Sub( const Vector4fw^ v );

		Vector4fw^	operator *= ( float scalar );
		Vector4fw^	operator *  ( float scalar );
		Vector4fw^	Mul( float scalar );

		Vector4fw^	operator *= ( const Vector4fw^ v );
		Vector4fw^	operator *  ( const Vector4fw^ v );
		Vector4fw^	Mul( const Vector4fw^ v );

		float		Length();
		float		LengthSquared();
		float		LengthManhattan();

		Vector4fw^	Normalize();

		float		Dot( const Vector4fw^ v );
		Vector4fw^	Cross( const Vector4fw^ v );

		float		Angle( const Vector4fw^ v );
	};
}}
