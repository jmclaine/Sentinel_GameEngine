#pragma once

#include "Color.h"
#include "WFloat.h"

using namespace Sentinel::Utilities;

namespace Sentinel { namespace Assets
{
	public ref class WColorRGBA
	{
	private:

		ColorRGBA*		mRef;

	public:

		WColorRGBA();
		WColorRGBA( float red, float green, float blue, float alpha );
		WColorRGBA( ColorRGBA* color );
		WColorRGBA( WColorRGBA^ color );
		WColorRGBA( WColorRGBA% color );
		~WColorRGBA();
		!WColorRGBA();

		ColorRGBA*		GetRef();

		WFloat^			R();
		WFloat^			G();
		WFloat^			B();
		WFloat^			A();

		float*			Ptr();

		UINT			ToUINT();
	};
}}
