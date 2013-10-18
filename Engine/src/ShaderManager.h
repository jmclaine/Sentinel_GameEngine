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

		// See 'config.xml' within the Shaders folder
		// for the structure of the shader definitions.
		//
		// returns true on success.
		//
		static bool LoadConfig( const char* filename, Renderer* renderer, ShaderManager* shaderManager );
	};
}
