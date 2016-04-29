#pragma once
/*
Create and set the SpriteSystem before calling Build()
*/
#include "Sentinel.h"
#include "SpriteSystem.h"
#include "Vector2.h"

namespace Sentinel
{
	class Font;

	class SENTINEL_DLL FontSystem
	{
	public:
		std::unique_ptr<SpriteSystem> mSpriteSystem;

		////////////////////////

		FontSystem() { }
		FontSystem(const FontSystem&) = delete;
		FontSystem& operator = (const FontSystem&) = delete;
		virtual ~FontSystem() { }

		////////////////////////

		virtual void Load(const char* filename) = 0;

		virtual Font* Build(UINT glyphWidth, UINT glyphHeight) = 0;

		////////////////////////

		virtual void Draw(const Font* font, char text, const ColorRGBA& color, const Matrix4x4& matWorld) = 0;

		void Clear()
		{
			mSpriteSystem->Clear();
		}

		void Present(Component::Camera* camera)
		{
			mSpriteSystem->Present(camera);
		}
	};

	extern SENTINEL_DLL FontSystem* BuildFontSystemFT();
}
