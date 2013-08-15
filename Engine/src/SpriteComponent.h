#pragma once

#include "Common.h"
#include "Texture.h"
#include "Util.h"
#include "ParticleSystem.h"
#include "DrawableComponent.h"

namespace Sentinel
{
	class SENTINEL_DLL SpriteComponent : public DrawableComponent
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
