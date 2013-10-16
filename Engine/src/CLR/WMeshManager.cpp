#include "WMeshManager.h"
#include "WMesh.h"
#include "WArchive.h"
#include "WRenderer.h"
#include "WShaderManager.h"
#include "WTextureManager.h"

namespace Sentinel { namespace Wrapped
{
	DEFINE_ASSET_MANAGER( Mesh );

	void WMeshManager::Save( WArchive^			archive, 
							 WRenderer^			renderer, 
							 WShaderManager^	shaderManager, 
							 WTextureManager^	textureManager )
	{
		mRef->Save( *archive, renderer, shaderManager, textureManager );
	}

	void WMeshManager::Load( WArchive^			archive, 
							 WRenderer^			renderer, 
							 WShaderManager^	shaderManager, 
							 WTextureManager^	textureManager )
	{
		mRef->Load( *archive, renderer, shaderManager, textureManager );
	}
}}
