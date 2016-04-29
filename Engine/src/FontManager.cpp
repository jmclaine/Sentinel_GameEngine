#include "FontManager.h"
#include "Archive.h"
#include "SpriteManager.h"
#include "MaterialManager.h"

namespace Sentinel
{
	FontManager::FontManager()
	{ }

	/////////////////////////////////

	void FontManager::Save(
		Archive& archive,
		SpriteManager* spriteManager,
		MaterialManager* materialManager)
	{
		UINT count = mData.size();
		archive.Write(&count);

		for (auto it : mData)
		{
			std::shared_ptr<Font> font = it.second;

			archive.Write(&it.first);

			Font::Save(archive, font.get(), spriteManager, materialManager);
		}
	}

	void FontManager::Load(
		Archive& archive,
		SpriteManager* spriteManager,
		MaterialManager* materialManager)
	{
		RemoveAll();

		UINT count;
		archive.Read(&count);

		std::string name;
		for (UINT x = 0; x < count; ++x)
		{
			archive.Read(&name);

			std::shared_ptr<Font> font = std::shared_ptr<Font>(Font::Load(archive, spriteManager, materialManager));

			Add(name, font);
		}
	}
}