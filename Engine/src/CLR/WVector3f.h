#pragma once

#include "Property.h"
#include "Vector3f.h"

namespace Sentinel { namespace Wrapped
{
	public ref class WVector3f
	{
		DECLARE_REF( Vector3f );

	public:

		WVector3f();
		WVector3f( float _x, float _y, float _z );
		WVector3f( const float* v );
		WVector3f( const Vector3f% v );
		WVector3f( const WVector3f% v );
		
		////////////////////////////////

		DECLARE_PROPERTY( float, x );
		DECLARE_PROPERTY( float, y );
		DECLARE_PROPERTY( float, z );

		////////////////////////////////

		DECLARE_OP_DEREF( Vector3f );

		bool		operator == ( WVector3f^ v );
		bool		operator != ( WVector3f^ v );
		bool		Equals( WVector3f^ v );
		
		WVector3f^	operator += ( WVector3f^ v );
		static WVector3f^ operator + ( WVector3f^ v0, WVector3f^ v1 );
		WVector3f^	Add( WVector3f^ v );

		WVector3f^	operator -= ( WVector3f^ v );
		static WVector3f^ operator - ( WVector3f^ v0, WVector3f^ v1 );
		WVector3f^	Sub( WVector3f^ v );

		WVector3f^	operator *= ( float scalar );
		static WVector3f^ operator * ( WVector3f^ v, float scalar );
		static WVector3f^ operator * ( float scalar, WVector3f^ v );
		WVector3f^	Mul( float scalar );

		WVector3f^	operator *= ( WVector3f^ v );
		static WVector3f^ operator * ( WVector3f^ v0, WVector3f^ v1 );
		WVector3f^	Mul( WVector3f^ v );

		float		Length();
		float		LengthSquared();
		float		LengthManhattan();

		WVector3f^	Normalize();

		float		Dot( WVector3f^ v );
		WVector3f^	Cross( WVector3f^ v );

		float		Angle( WVector3f^ v );
	};

	DECLARE_CLASS_REF( Vector3f );
}}
