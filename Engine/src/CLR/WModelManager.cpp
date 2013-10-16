#include "WModelManager.h"
#include "WModel.h"
#include "WArchive.h"
#include "WRenderer.h"
#include "WShaderManager.h"
#include "WTextureManager.h"

namespace Sentinel { namespace Wrapped
{
	DEFINE_ASSET_MANAGER( Model );

	void WModelManager::Save( WArchive^			archive, 
							 WRenderer^			renderer, 
							 WShaderManager^	shaderManager, 
							 WTextureManager^	textureManager )
	{
		mRef->Save( *archive, renderer, shaderManager, textureManager );
	}

	void WModelManager::Load( WArchive^			archive, 
							 WRenderer^			renderer, 
							 WShaderManager^	shaderManager, 
							 WTextureManager^	textureManager )
	{
		mRef->Load( *archive, renderer, shaderManager, textureManager );
	}
}}
