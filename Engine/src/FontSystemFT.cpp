/*
Loads font files supported by FreeType 2
http://www.freetype.org/
*/
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#ifdef NDEBUG
	#pragma comment (lib, "freetype251.lib")
#else
	#pragma comment (lib, "freetype251_D.lib")
#endif

#include "FontSystem.h"
#include "Renderer.h"
#include "Sprite.h"
#include "Texture.h"
#include "Material.h"
#include "Util.h"

namespace Sentinel
{
	class FontSystemFT : public FontSystem
	{
	private:

		FT_Library		mLibrary;
		FT_Face			mFace;
		
	public:

		FontSystemFT()
		{
			FT_Error error;

			// Initialize the FreeType font library.
			//
			error = FT_Init_FreeType( &mLibrary );
			if( error )
				throw AppException( "Error: Failed to create font library." );
		}

		~FontSystemFT()
		{
			FT_Done_FreeType( mLibrary );
		}

		void Load( const char* filename )
		{
			FT_Error error;

			// Access and load a single type of font.
			//
			error = FT_New_Face( mLibrary, filename, 0, &mFace );

			if( error == FT_Err_Unknown_File_Format )
				throw AppException( "Error: Unsupported font format." );
			else
			if( error )
				throw AppException( "Error: Failed to open font '" + std::string( filename ) + "'" );
		}

		std::shared_ptr< Font > Build( UINT glyphWidth, UINT glyphHeight )
		{
			FT_Error error;

			// Set the font size.
			//
			error = FT_Set_Char_Size( mFace, glyphWidth<<6, glyphHeight<<6, 96, 96 );
			if( error )
				return NULL;

			// Create font.
			//
			std::shared_ptr< Sprite > sprite( new Sprite() );

			std::shared_ptr< Font > font( new Font() );

			font->mSprite = sprite;

			// Set glyph texture cell to be within powers of 2.
			//
			glyphWidth  = NextPowerOf2( glyphWidth );
			glyphHeight = NextPowerOf2( glyphHeight );

			// 16x16 cells.
			//
			UINT textureWidth  = glyphWidth  << 4;
			UINT textureHeight = glyphHeight << 4;
			UINT textureSize   = ((textureWidth * textureHeight) << 2);	// RGBA

			float textureRatioX = glyphWidth  / static_cast< float >(textureWidth);
			float textureRatioY = glyphHeight / static_cast< float >(textureHeight);

			// Allocate memory for texture.
			//
			BYTE* pixels = (BYTE*)malloc( textureSize );
			ZeroMemory( pixels, textureSize );

			// Build texture from each glyph.
			//
			FT_Glyph glyph;

			for( FT_ULong x = 0; x < NUM_CHARS; ++x )
			{
				// Create the glyph.
				//
				error = FT_Load_Glyph( mFace, FT_Get_Char_Index( mFace, x ), FT_LOAD_DEFAULT );
				if( error )
					continue;

				error = FT_Get_Glyph( mFace->glyph, &glyph );
				if( error )
					continue;

				FT_Glyph_Metrics& metrics = mFace->glyph->metrics;

				// Set the metrics.
				//
				font->mOffsetX[ x ] = static_cast< float >(metrics.horiBearingX) / 64.0f;
				font->mOffsetY[ x ] = static_cast< float >(metrics.horiBearingY) / 64.0f;
				font->mAdvance[ x ] = static_cast< float >(metrics.horiAdvance) / 64.0f;
				
				// Convert to Bitmap.
				//
				error = FT_Glyph_To_Bitmap( &glyph, FT_RENDER_MODE_NORMAL, 0, 1 );
				if( error )
					continue;

				FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;
				FT_Bitmap& bitmap = bitmap_glyph->bitmap;

				// Add glyph frame to Sprite.
				//
				float posX = static_cast< float >(x & 15) * textureRatioX;
				float posY = static_cast< float >(x >> 4) * textureRatioY;

				sprite->AddFrame( Quad( posX, posY, posX + textureRatioX, posY + textureRatioY ));

				// Copy glyph to texture.
				//
				int bitmapWidth  = bitmap.width;
				int bitmapHeight = bitmap.rows;

				int index = 0;
				BYTE color = 0;

				int startX = (x & 15) * (glyphWidth << 2);
				int startY = (x >> 4) * (textureWidth << 2) * glyphHeight;

				for( int h = 0; h < (int)glyphHeight; ++h )
				{
					for( int w = 0; w < (int)glyphWidth; ++w )
					{
						index = (startX + (w << 2)) + (startY + h * (textureWidth << 2));

						color = (w >= bitmapWidth || h >= bitmapHeight) ? 0 : bitmap.buffer[ w + bitmapWidth*h ];

						pixels[ index ] = color;
						++index;

						pixels[ index ] = color;
						++index;

						pixels[ index ] = color;
						++index;

						pixels[ index ] = color;
					}
				}

				FT_Done_Glyph( glyph );
			}

			std::shared_ptr< Texture > texture( mSpriteSystem->GetRenderer()->CreateTextureFromMemory( pixels, textureWidth, textureHeight, IMAGE_FORMAT_RGBA ));

			free( pixels );

			if( texture.get() == NULL )
			{
				REPORT_ERROR( "Failed to create texture for font.", "Texture Creation Error" );

				return NULL;
			}

			std::shared_ptr< Material > material( new Material() );
			material->mTexture[ TEXTURE_DIFFUSE ] = texture;

			font->mMaterial = material;

			return font;
		}

		void Draw( char text, const ColorRGBA& color, const Matrix4f& matWorld )
		{
			mSpriteSystem->mSprite   = mFont->mSprite;
			mSpriteSystem->mMaterial = mFont->mMaterial;

			mSpriteSystem->Draw( text, color, matWorld );
		}
	};

	FontSystem* BuildFontSystemFT()
	{
		return new FontSystemFT();
	}
}
