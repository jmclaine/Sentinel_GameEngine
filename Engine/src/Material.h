#pragma once

#include "Common.h"
#include "ColorRGBA.h"

namespace Sentinel
{
	class SENTINEL_DLL Material
	{
	public:

		ColorRGBA			mAmbient;
		ColorRGBA			mDiffuse;
		ColorRGBA			mSpecular;
		float				mSpecularComponent;

		//////////////////////////////////

		Material( const ColorRGBA& ambient  = ColorRGBA( 0.2f, 0.2f, 0.2f, 1.0f ),
				  const ColorRGBA& diffuse  = ColorRGBA( 0.6f, 0.6f, 0.6f, 1.0f ),
				  const ColorRGBA& specular = ColorRGBA( 0.2f, 0.2f, 0.2f, 1.0f ),
				  float spec_comp = 32.0f );
	};
}
