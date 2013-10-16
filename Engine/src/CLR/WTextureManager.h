#pragma once

#include "Property.h"
#include "WAssetManager.h"
#include "TextureManager.h"

namespace Sentinel { namespace Wrapped
{
	ref class WArchive;
	ref class WTexture;
	ref class WRenderer;

	public ref class WTextureManager
	{
		DECLARE_ASSET_MANAGER( Texture );

	public:

		void	Save( WArchive^ archive, WRenderer^ renderer );
		void	Load( WArchive^ archive, WRenderer^ renderer );
	};
}}
