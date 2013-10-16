#pragma once

#include "AssetManager.h"
#include "Mesh.h"

namespace Sentinel
{
	class Archive;
	class Renderer;
	class ShaderManager;
	class TextureManager;

	class SENTINEL_DLL MeshManager : public AssetManager< Mesh >
	{
	public:

		MeshManager();
		~MeshManager();

		/////////////////////////////////

		void Save( Archive&			archive, 
				   Renderer*		renderer, 
				   ShaderManager*	shaderManager, 
				   TextureManager*	textureManager );

		void Load( Archive&			archive, 
				   Renderer*		renderer, 
				   ShaderManager*	shaderManager, 
				   TextureManager*	textureManager );
	};
}
