#pragma once

#include "AssetManager.h"
#include "Font.h"

namespace Sentinel
{
	class Archive;
	class SpriteManager;
	class MaterialManager;

	class SENTINEL_DLL FontManager : public AssetManager<Font>
	{
	public:
		FontManager();

		FontManager(const FontManager&) = delete;
		FontManager& operator = (const FontManager&) = delete;

		/////////////////////////////////

		void Save(
			Archive& archive,
			SpriteManager* spriteManager,
			MaterialManager* materialManager);

		void Load(
			Archive& archive,
			SpriteManager* spriteManager,
			MaterialManager* materialManager);
	};
}
