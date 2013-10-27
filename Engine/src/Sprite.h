#pragma once

#include <memory>

#include "ColorRGBA.h"
#include "Point.h"

namespace Sentinel
{
	class Texture;
	class Shader;
	class Vector2f;

	class SENTINEL_DLL Sprite
	{
	private:

		Point2i						mSize;
		Point2i						mDimension;

		UINT						mFrame;
		UINT						mNumFrames;

		std::shared_ptr< Texture >	mTexture;
		std::shared_ptr< Shader >	mShader;

	public:

		ColorRGBA					mColor;

		//////////////////////////////////////

		Sprite( std::shared_ptr< Texture > texture, std::shared_ptr< Shader > shader, const Point2i& spriteSize );
		~Sprite();

		//////////////////////////////////////

		const Point2i&				GetSize();
		const Point2i&				GetDimension();

		void						SetFrame( UINT frame );
		UINT						GetFrame();
		Vector2f					GetFrameCoords();
		Vector2f					GetFrameCoords( UINT frame );

		UINT						NumFrames();

		std::shared_ptr< Texture >	GetTexture();
		std::shared_ptr< Shader >	GetShader();
	};
}
