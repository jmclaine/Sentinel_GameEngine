#pragma once

#include "Vector2f.h"
#include "WFloat.h"

using namespace Sentinel::Utilities;

namespace Sentinel { namespace Math
{
	public ref class WVector2f
	{
	private:

		Vector2f*	mRef;

	public:

		WVector2f();
		WVector2f( float _x, float _y );
		WVector2f( const float* v );
		WVector2f( const Vector2f% v );
		WVector2f( const WVector2f% v );
		~WVector2f();
		!WVector2f();

		Vector2f*	GetRef();

		WFloat^		X();
		WFloat^		Y();
		
		bool		operator == ( const WVector2f^ v );
		bool		operator != ( const WVector2f^ v );
		bool		Equals( const WVector2f^ v );
		
		WVector2f^	operator += ( const WVector2f^ v );
		WVector2f^	operator +  ( const WVector2f^ v );
		WVector2f^	Add( const WVector2f^ v );

		WVector2f^	operator -= ( const WVector2f^ v );
		WVector2f^	operator -  ( const WVector2f^ v );
		WVector2f^	Sub( const WVector2f^ v );

		WVector2f^	operator *= ( float scalar );
		WVector2f^	operator *  ( float scalar );
		WVector2f^	Mul( float scalar );

		WVector2f^	operator *= ( const WVector2f^ v );
		WVector2f^	operator *  ( const WVector2f^ v );
		WVector2f^	Mul( const WVector2f^ v );

		float		Length();
		float		LengthSquared();
		float		LengthManhattan();

		WVector2f^	Normalize();

		float		Dot( const WVector2f^ v );
		WVector2f^	Cross();

		float		Angle( const WVector2f^ v );
	};
}}
