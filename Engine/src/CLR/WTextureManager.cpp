#include "WTextureManager.h"
#include "WTexture.h"
#include "WRenderer.h"
#include "WArchive.h"

namespace Sentinel { namespace Wrapped
{
	DEFINE_ASSET_MANAGER( Texture );

	void WTextureManager::Save( WArchive^ archive, WRenderer^ renderer )
	{
		mRef->Save( *archive, renderer );
	}

	void WTextureManager::Load( WArchive^ archive, WRenderer^ renderer )
	{
		mRef->Load( *archive, renderer );
	}
}}
