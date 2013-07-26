#pragma once

#include "Texture.h"
#include "Util.h"
#include "ParticleSystem.h"

namespace Sentinel
{
	class SpriteComponent : public DrawableComponent
	{
		friend class ParticleSystem;

	private:

		TransformComponent*		mTransform;
		
		Texture*				mTexture;
		POINT					mSpriteSize;
		POINT					mSpriteDimension;

		UINT					mNumFrames;

		ColorRGBA				mColor;
		UINT					mFrame;

	public:

		SpriteComponent( Texture* texture, const POINT& spriteSize );

		void Startup();

		void Update();

		void Shutdown();

		/////////////////////////////////

		void SetColor( const ColorRGBA& color );
		void SetFrame( UINT frame );
	};
}
