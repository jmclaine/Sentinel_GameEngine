#pragma once

#include "AssetManager.h"
#include "Shader.h"

namespace Sentinel
{
	class Renderer;
	class Archive;

	class SENTINEL_DLL ShaderManager : public AssetManager< Shader >
	{
	public:

		ShaderManager();
		~ShaderManager();

		/////////////////////////////////

		void Save( Archive& archive );

		void Load( Archive& archive, Renderer* renderer );

		static ShaderManager* LoadConfig( const char* filename, Renderer* renderer, ShaderManager* shaderManager = NULL );
	};
}
