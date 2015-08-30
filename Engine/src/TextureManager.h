#pragma once

#include "AssetManager.h"
#include "Texture.h"

namespace Sentinel
{
	class Archive;
	class Renderer;

	class SENTINEL_DLL TextureManager : public AssetManager <Texture>
	{
	public:

		// Uses Renderer to store default textures
		// NULL_TEXTURE and BASE_TEXTURE
		//
		TextureManager(Renderer* renderer);
		~TextureManager();

		/////////////////////////////////

		void Save(Archive& archive, Renderer* renderer);

		void Load(Archive& archive, Renderer* renderer);
	};
}
