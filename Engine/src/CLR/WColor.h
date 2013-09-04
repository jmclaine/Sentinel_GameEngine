#pragma once

#include "Color.h"
#include "Property.h"

namespace Sentinel { namespace Assets
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
	};

	DECLARE_CLASS_REF( ColorRGBA );
}}
