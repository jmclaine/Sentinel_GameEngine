#include "Colorw.h"

namespace Sentinel { namespace Assets
{
	ColorRGBAw::ColorRGBAw()
	{
		mRef = new ColorRGBA();
	}

	ColorRGBAw::ColorRGBAw( ColorRGBAw% color )
	{
		mRef = new ColorRGBA( *color.mRef );
	}

	ColorRGBAw::ColorRGBAw( ColorRGBA& color )
	{
		mRef = new ColorRGBA( color );
	}

	ColorRGBAw::~ColorRGBAw()
	{
		delete mRef;
	}

	ColorRGBA* ColorRGBAw::GetRef()
	{
		return mRef;
	}

	float ColorRGBAw::R()
	{
		return mRef->R();
	}

	float ColorRGBAw::G()
	{
		return mRef->G();
	}

	float ColorRGBAw::B()
	{
		return mRef->B();
	}

	float ColorRGBAw::A()
	{
		return mRef->A();
	}

	void ColorRGBAw::SetR( float red )
	{
		return mRef->SetR( red );
	}

	void ColorRGBAw::SetG( float green )
	{
		return mRef->SetG( green );
	}

	void ColorRGBAw::SetB( float blue )
	{
		return mRef->SetB( blue );
	}

	void ColorRGBAw::SetA( float alpha )
	{
		return mRef->SetA( alpha );
	}

	float*	ColorRGBAw::Ptr()
	{
		return mRef->Ptr();
	}
}}