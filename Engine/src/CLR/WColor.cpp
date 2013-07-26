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

	WColorRGBA::WColorRGBA( WColorRGBA% color )
	{
		mRef = new ColorRGBA( *color.mRef );
	}

	WColorRGBA::WColorRGBA( ColorRGBA& color )
	{
		mRef = new ColorRGBA( color );
	}

	WColorRGBA::~WColorRGBA()
	{
		delete mRef;
	}

	ColorRGBA* WColorRGBA::GetRef()
	{
		return mRef;
	}

	float WColorRGBA::R()
	{
		return mRef->R();
	}

	float WColorRGBA::G()
	{
		return mRef->G();
	}

	float WColorRGBA::B()
	{
		return mRef->B();
	}

	float WColorRGBA::A()
	{
		return mRef->A();
	}

	void WColorRGBA::SetR( float red )
	{
		return mRef->SetR( red );
	}

	void WColorRGBA::SetG( float green )
	{
		return mRef->SetG( green );
	}

	void WColorRGBA::SetB( float blue )
	{
		return mRef->SetB( blue );
	}

	void WColorRGBA::SetA( float alpha )
	{
		return mRef->SetA( alpha );
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