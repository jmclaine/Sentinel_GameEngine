#pragma once

#include "AssetManager.h"
#include "Texture.h"

namespace Sentinel
{
	class Archive;
	class Renderer;

	class SENTINEL_DLL TextureManager : public AssetManager< Texture >
	{	
	public:

		TextureManager();
		~TextureManager();

		/////////////////////////////////

		void Save( Archive& archive, Renderer* renderer );

		void Load( Archive& archive, Renderer* renderer );
	};
}
