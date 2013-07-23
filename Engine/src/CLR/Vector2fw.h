#pragma once

#include "Vector2f.h"

using namespace System;

namespace Sentinel { namespace Math
{
	public ref class Vector2fw
	{
	private:

		Vector2f*	mRef;

	public:

		Vector2fw();
		Vector2fw( float _x, float _y );
		Vector2fw( const float* v );
		Vector2fw( const Vector2f% v );
		Vector2fw( const Vector2fw% v );
		~Vector2fw();

		const Vector2f*	GetRef();

		float		X();
		float		Y();
		
		void		SetX( float _x );
		void		SetY( float _y );
		
		bool		operator == ( const Vector2fw^ v );
		bool		operator != ( const Vector2fw^ v );
		bool		Equals( const Vector2fw^ v );
		
		Vector2fw^	operator += ( const Vector2fw^ v );
		Vector2fw^	operator +  ( const Vector2fw^ v );
		Vector2fw^	Add( const Vector2fw^ v );

		Vector2fw^	operator -= ( const Vector2fw^ v );
		Vector2fw^	operator -  ( const Vector2fw^ v );
		Vector2fw^	Sub( const Vector2fw^ v );

		Vector2fw^	operator *= ( float scalar );
		Vector2fw^	operator *  ( float scalar );
		Vector2fw^	Mul( float scalar );

		Vector2fw^	operator *= ( const Vector2fw^ v );
		Vector2fw^	operator *  ( const Vector2fw^ v );
		Vector2fw^	Mul( const Vector2fw^ v );

		float		Length();
		float		LengthSquared();
		float		LengthManhattan();

		Vector2fw^	Normalize();

		float		Dot( const Vector2fw^ v );
		Vector2fw^	Cross();

		float		Angle( const Vector2fw^ v );
	};
}}
