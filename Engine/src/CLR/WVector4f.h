#pragma once

#include "Property.h"
#include "Vector4f.h"

namespace Sentinel { namespace Wrapped
{
	public ref class WVector4f
	{
		DECLARE_REF( Vector4f );

	public:

		WVector4f();
		WVector4f( float _x, float _y, float _z, float _w );
		WVector4f( const float* v );
		WVector4f( const Vector4f% v );
		WVector4f( const WVector4f% v );

		////////////////////////////////

		DECLARE_PROPERTY( float, x );
		DECLARE_PROPERTY( float, y );
		DECLARE_PROPERTY( float, z );
		DECLARE_PROPERTY( float, w );

		////////////////////////////////

		DECLARE_OP_DEREF( Vector4f );

		bool		operator == ( WVector4f^ v );
		bool		operator != ( WVector4f^ v );
		bool		Equals( WVector4f^ v );
		
		WVector4f^	operator += ( WVector4f^ v );
		static WVector4f^ operator + ( WVector4f^ v0, WVector4f^ v1 );
		WVector4f^	Add( WVector4f^ v );

		WVector4f^	operator -= ( WVector4f^ v );
		static WVector4f^ operator - ( WVector4f^ v0, WVector4f^ v1 );
		WVector4f^	Sub( WVector4f^ v );

		WVector4f^	operator *= ( float scalar );
		static WVector4f^ operator * ( WVector4f^ v, float scalar );
		static WVector4f^ operator * ( float scalar, WVector4f^ v );
		WVector4f^	Mul( float scalar );

		WVector4f^	operator *= ( WVector4f^ v );
		static WVector4f^ operator * ( WVector4f^ v0, WVector4f^ v1 );
		WVector4f^	Mul( WVector4f^ v );

		float		Length();
		float		LengthSquared();
		float		LengthManhattan();

		WVector4f^	Normalize();

		float		Dot( WVector4f^ v );
		WVector4f^	Cross( WVector4f^ v );

		float		Angle( WVector4f^ v );
	};

	DECLARE_CLASS_REF( Vector4f );
}}
