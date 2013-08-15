#include "Color.h"

namespace Sentinel
{
	ColorRGBA::ColorRGBA() :
		r( 1 ), g( 1 ), b( 1 ), a( 1 )
	{}

	ColorRGBA::ColorRGBA( float red, float green, float blue, float alpha ) :
		r( red ), g( green ), b( blue ), a( alpha )
	{}

	float* ColorRGBA::Ptr()
	{
		return static_cast< float* >(&r);
	}

	UINT ColorRGBA::ToUINT() const
	{
		return \
			((UINT)(a * 255.0f) << 24) |
			((UINT)(b * 255.0f) << 16) |
			((UINT)(g * 255.0f) << 8)  |
			((UINT)(r * 255.0f));
	}
}
