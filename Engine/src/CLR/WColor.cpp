#include "WColor.h"

namespace Sentinel { namespace Assets
{
	DEFINE_REF( ColorRGBA );

	WColorRGBA::WColorRGBA()
	{
		mRef = new ColorRGBA();
	}

	WColorRGBA::WColorRGBA( float red, float green, float blue, float alpha )
	{
		mRef = new ColorRGBA( red, green, blue, alpha );
	}

	WColorRGBA::WColorRGBA( ColorRGBA& color )
	{
		mRef = new ColorRGBA( color );
	}

	WColorRGBA::WColorRGBA( WColorRGBA^ color )
	{
		mRef = new ColorRGBA( *color->mRef );
	}

	WColorRGBA::WColorRGBA( WColorRGBA% color )
	{
		mRef = new ColorRGBA( *color.mRef );
	}

	////////////////////////////////

	DEFINE_PROPERTY( ColorRGBA, float, r );
	DEFINE_PROPERTY( ColorRGBA, float, g );
	DEFINE_PROPERTY( ColorRGBA, float, b );
	DEFINE_PROPERTY( ColorRGBA, float, a );

	////////////////////////////////

	DEFINE_OP_DEREF( ColorRGBA );

	float* WColorRGBA::Ptr()
	{
		return mRef->Ptr();
	}

	UINT WColorRGBA::ToUINT()
	{
		return mRef->ToUINT();
	}

	////////////////////////////////

	DEFINE_CLASS_REF( ColorRGBA );
}}