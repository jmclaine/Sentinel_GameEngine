/*
Loads font files supported by FreeType 2
http://www.freetype.org/
*/
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#ifdef NDEBUG
#pragma comment (lib, "Release/freetype.lib")
#else
#pragma comment (lib, "Debug/freetype.lib")
#endif

#include "FontSystem.h"
#include "Renderer.h"
#include "Sprite.h"
#include "Texture.h"
#include "Material.h"
#include "Exception.h"
#include "Debug.h"
#include "Font.h"

namespace Sentinel
{
	class FontSystemFT : public FontSystem
	{
	private:

		FT_Library mLibrary;
		FT_Face mFace;

	public:

		FontSystemFT()
		{
			FT_Error error;

			error = FT_Init_FreeType(&mLibrary);
			if (error)
				throw AppException("Error: Failed to create font library.");
		}

		~FontSystemFT()
		{
			FT_Done_FreeType(mLibrary);
		}

		void Load(const char* filename)
		{
			FT_Error error;

			error = FT_New_Face(mLibrary, filename, 0, &mFace);
			
			if (error == FT_Err_Unknown_File_Format)
				throw AppException("Error: Unsupported font format '" + std::string(filename) + "'");
			else if (error)
				throw AppException("Error: Failed to open font '" + std::string(filename) + "'");
		}

		Font* Build(UINT glyphWidth, UINT glyphHeight)
		{
			FT_Error error;

			error = FT_Set_Char_Size(mFace, glyphWidth << 6, glyphHeight << 6, 96, 96);
			if (error)
				throw std::exception("Failed to create font.");

			Font* font(new Font());
			std::shared_ptr<Sprite> sprite(new Sprite());

			font->mSprite = sprite;
			font->mSize = Vector2((float)glyphWidth, (float)glyphHeight);

			UINT textureWidth = glyphWidth << 4;
			UINT textureHeight = glyphHeight << 4;
			UINT textureSize = ((textureWidth * textureHeight) << 2); // RGBA

			float textureRatioX = glyphWidth / static_cast<float>(textureWidth);
			float textureRatioY = glyphHeight / static_cast<float>(textureHeight);

			BYTE* pixels = (BYTE*)malloc(textureSize);
			memset(pixels, 0, textureSize);

			FT_Glyph glyph;

			for (FT_ULong x = 0; x < NUM_CHARS; ++x)
			{
				error = FT_Load_Glyph(mFace, FT_Get_Char_Index(mFace, x), FT_LOAD_DEFAULT);
				if (error)
					continue;

				error = FT_Get_Glyph(mFace->glyph, &glyph);
				if (error)
					continue;

				FT_Glyph_Metrics& metrics = mFace->glyph->metrics;

				font->mOffsetX[x] = static_cast<float>(metrics.horiBearingX) / 64.0f;
				font->mOffsetY[x] = static_cast<float>(metrics.horiBearingY) / 64.0f;
				font->mAdvance[x] = static_cast<float>(metrics.horiAdvance) / 64.0f;

				error = FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, 0, 1);
				if (error)
					continue;

				FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;
				FT_Bitmap& bitmap = bitmap_glyph->bitmap;

				float posX = static_cast<float>(x & 15) * textureRatioX;
				float posY = static_cast<float>(x >> 4) * textureRatioY;

				sprite->AddFrame(Quad(posX, posY, posX + textureRatioX, posY + textureRatioY));

				int bitmapWidth = bitmap.width;
				int bitmapHeight = bitmap.rows;

				int index = 0;
				BYTE color = 0;

				int startX = (x & 15) * (glyphWidth << 2);
				int startY = (x >> 4) * (textureWidth << 2) * glyphHeight;

				for (int h = 0; h < (int)glyphHeight; ++h)
				{
					for (int w = 0; w < (int)glyphWidth; ++w)
					{
						index = (startX + (w << 2)) + (startY + h * (textureWidth << 2));

						color = (w >= bitmapWidth || h >= bitmapHeight) ? 0 : bitmap.buffer[w + bitmapWidth*h];

						pixels[index] = color;
						++index;

						pixels[index] = color;
						++index;

						pixels[index] = color;
						++index;

						pixels[index] = color;
					}
				}

				FT_Done_Glyph(glyph);
			}
			
			std::shared_ptr<Texture> texture(mSpriteSystem->GetRenderer()->CreateTexture(pixels, textureWidth, textureHeight, ImageFormat::RGBA));
			free(pixels);
			font->mTexture = texture;

			std::shared_ptr<Material> material(new Material());
			material->mTexture[(BYTE)TextureIndex::DIFFUSE] = texture;
			font->mMaterial = material;

			return font;
		}

		void Draw(const Font* font, char text, const ColorRGBA& color, const Matrix4x4& matWorld)
		{
			mSpriteSystem->mSprite = font->mSprite;
			mSpriteSystem->mMaterial = font->mMaterial;

			mSpriteSystem->Draw(text, color, matWorld);
		}
	};

	FontSystem* BuildFontSystemFT()
	{
		return new FontSystemFT();
	}
}
