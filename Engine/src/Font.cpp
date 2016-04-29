#include "Font.h"
#include "Archive.h"
#include "Sprite.h"
#include "Material.h"
#include "SpriteManager.h"
#include "MaterialManager.h"

namespace Sentinel
{
	Font::Font()
	{ }

	void Font::Save(
		Archive& archive,
		Font* font,
		SpriteManager* spriteManager,
		MaterialManager* materialManager)
	{
		_ASSERT(0);
		//std::string sprite = spriteManager->Get(font->mSprite);
		//archive.Write(&sprite);

		//std::string material = materialManager->Get(font->mMaterial);
		//archive.Write(&material);

		archive.Write(&font->mSize);
	}

	Font* Font::Load(
		Archive& archive,
		SpriteManager* spriteManager,
		MaterialManager* materialManager)
	{
		_ASSERT(0);
		Font* font = new Font();

		std::string sprite;
		archive.Read(&sprite);
		//font->mSprite = spriteManager->Get(sprite);

		std::string material;
		archive.Read(&material);
		//font->mMaterial = materialManager->Get(material);

		archive.Read(&font->mSize);

		return font;
	}
}