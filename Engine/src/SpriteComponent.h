#pragma once

namespace Sentinel
{
	class SpriteComponent : public GameComponent
	{
	public:

		Texture*	mTexture;
		vec2i		mSpriteSize;
		vec2i		mSpriteDimension;

		Material	mMaterial;

		UINT		mFrame;
		UINT		mNumFrames;

		SpriteComponent();

		void Startup();

		void Update();

		void Shutdown();
	};
}
