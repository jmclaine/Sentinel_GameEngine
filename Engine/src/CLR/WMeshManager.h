#pragma once

#include "Property.h"
#include "WAssetManager.h"
#include "MeshManager.h"

namespace Sentinel { namespace Wrapped
{
	ref class WArchive;
	ref class WMesh;
	ref class WRenderer;
	ref class WShaderManager;
	ref class WTextureManager;

	public ref class WMeshManager
	{
		DECLARE_ASSET_MANAGER( Mesh );

	public:

		void	Save( WArchive^			archive, 
					  WRenderer^		renderer, 
					  WShaderManager^	shaderManager, 
					  WTextureManager^	textureManager );

		void	Load( WArchive^			archive, 
					  WRenderer^		renderer, 
					  WShaderManager^	shaderManager, 
					  WTextureManager^	textureManager );
	};
}}
