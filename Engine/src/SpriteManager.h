#pragma once

#include "AssetManager.h"
#include "Sprite.h"

namespace Sentinel
{
	class Archive;
	class Renderer;
	class TextureManager;

	class SENTINEL_DLL SpriteManager : public AssetManager< Sprite >
	{
	public:

		SpriteManager();
		~SpriteManager();

		/////////////////////////////////

		void Save( Archive&			archive );

		void Load( Archive&			archive );
	};
}
