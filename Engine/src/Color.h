#pragma once

#include "Common.h"

namespace Sentinel
{
	class SENTINEL_DLL ColorRGBA
	{
	public:

		float r, g, b, a;

		/////////////////////////////

		ColorRGBA();
		ColorRGBA( float red, float green, float blue, float alpha = 1.0f );

		float*	Ptr();

		UINT	ToUINT() const;
	};
}
