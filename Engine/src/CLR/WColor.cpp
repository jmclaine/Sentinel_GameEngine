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

	WColorRGBA::WColorRGBA( ColorRGBA* color )
	{
		mRef = color;
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
		delete mRef;
	}

	WColorRGBA::!WColorRGBA()
	{
		delete mRef;
	}

	ColorRGBA* WColorRGBA::GetRef()
	{
		return mRef;
	}

	WFloat^ WColorRGBA::R()
	{
		return gcnew WFloat( &mRef->r );
	}

	WFloat^ WColorRGBA::G()
	{
		return gcnew WFloat( &mRef->g );
	}

	WFloat^ WColorRGBA::B()
	{
		return gcnew WFloat( &mRef->b );
	}

	WFloat^ WColorRGBA::A()
	{
		return gcnew WFloat( &mRef->a );
	}

	float* WColorRGBA::Ptr()
	{
		return mRef->Ptr();
	}

	UINT WColorRGBA::ToUINT()
	{
		return mRef->ToUINT();
	}
}}