#pragma once

#include "Property.h"
#include "WAssetManager.h"
#include "ModelManager.h"

namespace Sentinel { namespace Wrapped
{
	ref class WArchive;
	ref class WModel;
	ref class WRenderer;
	ref class WShaderManager;
	ref class WTextureManager;

	public ref class WModelManager
	{
		DECLARE_ASSET_MANAGER( Model );

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
