#pragma once

#include "AssetManager.h"
#include "Material.h"

namespace Sentinel
{
	class Archive;
	class ShaderManager;
	class TextureManager;

	class SENTINEL_DLL MaterialManager : public AssetManager< Material >
	{	
	public:

		MaterialManager();
		~MaterialManager();

		/////////////////////////////////

		void Save( Archive&			archive, 
				   ShaderManager*	shaderManager, 
				   TextureManager*	textureManager );

		void Load( Archive&			archive, 
				   ShaderManager*	shaderManager, 
				   TextureManager*	textureManager );
	};
}
