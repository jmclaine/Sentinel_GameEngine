#include "WColor.h"

namespace Sentinel { namespace Assets
{
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

	WColorRGBA::~WColorRGBA()
	{
		Delete();
	}

	WColorRGBA::!WColorRGBA()
	{
		Delete();
	}

	void WColorRGBA::Delete()
	{
		delete mRef;
	}

	ColorRGBA* WColorRGBA::GetRef()
	{
		return mRef;
	}

	////////////////////////////////

	DEFINE_PROPERTY( ColorRGBA, float, r );
	DEFINE_PROPERTY( ColorRGBA, float, g );
	DEFINE_PROPERTY( ColorRGBA, float, b );
	DEFINE_PROPERTY( ColorRGBA, float, a );

	////////////////////////////////

	float* WColorRGBA::Ptr()
	{
		return mRef->Ptr();
	}

	UINT WColorRGBA::ToUINT()
	{
		return mRef->ToUINT();
	}

	////////////////////////////////

	RColorRGBA::RColorRGBA( ColorRGBA* color )
	{
		mRef = color;
	}

	RColorRGBA::RColorRGBA( WColorRGBA^ color )
	{
		mRef = color->GetRef();
	}

	void RColorRGBA::Set( const ColorRGBA& color )
	{
		*mRef = color;
	}

	void RColorRGBA::Set( WColorRGBA^ color )
	{
		*mRef = *color->GetRef();
	}

	void RColorRGBA::Delete()
	{}
}}