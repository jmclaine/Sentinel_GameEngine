#pragma once

#include "Common.h"
#include "AssetManager.h"
#include "Texture.h"
#include "Singleton.h"

namespace Sentinel
{
	class SENTINEL_DLL TextureManager : public AssetManager< Texture >, public SingletonSafe< TextureManager >
	{
		friend class SingletonSafe< TextureManager >;
		friend class AssetManager< Texture >;
		
	private:

		TextureManager();
		~TextureManager();

	public:

		static TextureManager* Create();

		void Save( Archive& archive );

		void Load( Archive& archive );
	};
}
