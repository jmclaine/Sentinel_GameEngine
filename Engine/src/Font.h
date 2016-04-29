#pragma once

#include <memory>

#include "Vector2.h"

#define NUM_CHARS 256

namespace Sentinel
{
	class Archive;
	class Sprite;
	class Material;
	class Texture;
	class SpriteManager;
	class MaterialManager;

	class SENTINEL_DLL Font
	{
	public:
		std::shared_ptr<Sprite> mSprite;
		std::shared_ptr<Material> mMaterial;
		std::shared_ptr<Texture> mTexture;

		Vector2 mSize;

		float mOffsetX[NUM_CHARS];	// image offset
		float mOffsetY[NUM_CHARS];
		float mAdvance[NUM_CHARS];	// next character starting position (x-axis)

		/////////////////////

		Font();

		/////////////////////

		static void Save(
			Archive& archive,
			Font* font,
			SpriteManager* spriteManager,
			MaterialManager* materialManager);

		static Font* Load(
			Archive& archive,
			SpriteManager* spriteManager,
			MaterialManager* materialManager);
	};
}
