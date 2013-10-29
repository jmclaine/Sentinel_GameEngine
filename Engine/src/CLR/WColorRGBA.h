#pragma once

#include "Property.h"
#include "ColorRGBA.h"

namespace Sentinel { namespace Wrapped
{
	public ref class WColorRGBA
	{
		DECLARE_REF( ColorRGBA );
	
	public:

		WColorRGBA();
		WColorRGBA( float red, float green, float blue, float alpha );
		WColorRGBA( ColorRGBA& color );
		WColorRGBA( WColorRGBA^ color );
		WColorRGBA( WColorRGBA% color );

		////////////////////////////////

		DECLARE_PROPERTY( float, r );
		DECLARE_PROPERTY( float, g );
		DECLARE_PROPERTY( float, b );
		DECLARE_PROPERTY( float, a );

		////////////////////////////////

		DECLARE_OP_DEREF( ColorRGBA );

		float*		Ptr();

		UINT		ToUINT();

		////////////////////////////////

		bool		operator == ( WColorRGBA^ v );
		bool		operator != ( WColorRGBA^ v );
		bool		Equals( WColorRGBA^ v );

		WColorRGBA^	operator += ( WColorRGBA^ v );
		static WColorRGBA^ operator + ( WColorRGBA^ v0, WColorRGBA^ v1 );
		WColorRGBA^	Add( WColorRGBA^ v );

		WColorRGBA^	operator -= ( WColorRGBA^ v );
		static WColorRGBA^ operator - ( WColorRGBA^ v0, WColorRGBA^ v1 );
		WColorRGBA^	Sub( WColorRGBA^ v );

		WColorRGBA^	operator *= ( float scalar );
		static WColorRGBA^ operator * ( WColorRGBA^ v, float scalar );
		static WColorRGBA^ operator * ( float scalar, WColorRGBA^ v );
		WColorRGBA^	Mul( float scalar );

		WColorRGBA^	operator *= ( WColorRGBA^ v );
		static WColorRGBA^ operator * ( WColorRGBA^ v0, WColorRGBA^ v1 );
		WColorRGBA^	Mul( WColorRGBA^ v );
	};

	DECLARE_CLASS_REF( ColorRGBA );
}}
