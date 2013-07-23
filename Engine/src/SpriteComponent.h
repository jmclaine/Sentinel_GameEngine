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

	public:

		CameraComponent*		mCamera;

		ColorRGBA				mColor;
		UINT					mFrame;

		/////////////////////////////////

		SpriteComponent( Texture* texture, const POINT& spriteSize );

		void Startup();

		void Update();

		void Shutdown();
	};
}
