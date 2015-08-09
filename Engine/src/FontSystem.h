#pragma once
/*
Create and set the SpriteSystem before calling Build()
*/
#include "Common.h"
#include "SpriteSystem.h"
#include "Vector2f.h"

#define NUM_CHARS 256

namespace Sentinel
{
	class Font
	{
	public:

		std::shared_ptr< Sprite >   mSprite;
		std::shared_ptr< Material > mMaterial;

		Vector2f	mSize;
		float		mOffsetX[ NUM_CHARS ];	// image offset
		float		mOffsetY[ NUM_CHARS ];
		float		mAdvance[ NUM_CHARS ];	// next character starting position (x-axis)

		/////////////////////

		Font() {}
		virtual ~Font() {}
	};

	//////////////////////////////////////////

	class SENTINEL_DLL FontSystem
	{
	public:

		std::shared_ptr< SpriteSystem > mSpriteSystem;

		Matrix4f				mMatrixWVP;

		std::shared_ptr< Font > mFont;

		////////////////////////

		virtual ~FontSystem()
		{}

		////////////////////////

		virtual void Load( const char* filename ) = 0;

		virtual std::shared_ptr< Font > Build( UINT glyphWidth, UINT glyphHeight ) = 0;

		////////////////////////

		virtual void Draw( char text, const ColorRGBA& color, const Matrix4f& matWorld ) = 0;

		void Clear()
		{
			mSpriteSystem->Clear();
		}

		void Present()
		{
			mSpriteSystem->Present();
		}
	};

	extern SENTINEL_DLL FontSystem* BuildFontSystemFT();
}
