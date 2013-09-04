#pragma once

#include "Vector2f.h"
#include "Property.h"

namespace Sentinel { namespace Math
{
	public ref class WVector2f
	{
		DECLARE_REF( Vector2f );

	public:

		WVector2f();
		WVector2f( float _x, float _y );
		WVector2f( const float* v );
		WVector2f( const Vector2f% v );
		WVector2f( const WVector2f% v );

		////////////////////////////////

		DECLARE_PROPERTY( float, x );
		DECLARE_PROPERTY( float, y );

		////////////////////////////////

		DECLARE_OP_DEREF( Vector2f );
		
		bool		operator == ( WVector2f^ v );
		bool		operator != ( WVector2f^ v );
		bool		Equals( WVector2f^ v );
		
		WVector2f^	operator += ( WVector2f^ v );
		static WVector2f^ operator + ( WVector2f^ v0, WVector2f^ v1 );
		WVector2f^	Add( WVector2f^ v );

		WVector2f^	operator -= ( WVector2f^ v );
		static WVector2f^ operator - ( WVector2f^ v0, WVector2f^ v1 );
		WVector2f^	Sub( WVector2f^ v );

		WVector2f^	operator *= ( float scalar );
		static WVector2f^ operator * ( WVector2f^ v, float scalar );
		static WVector2f^ operator * ( float scalar, WVector2f^ v );
		WVector2f^	Mul( float scalar );

		WVector2f^	operator *= ( WVector2f^ v );
		static WVector2f^ operator * ( WVector2f^ v0, WVector2f^ v1 );
		WVector2f^	Mul( WVector2f^ v );

		float		Length();
		float		LengthSquared();
		float		LengthManhattan();

		WVector2f^	Normalize();

		float		Dot( WVector2f^ v );
		WVector2f^	Cross();

		float		Angle( WVector2f^ v );
	};

	DECLARE_CLASS_REF( Vector2f );
}}
