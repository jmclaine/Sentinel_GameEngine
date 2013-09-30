#pragma once

#include "Texture.h"
#include "Util.h"
#include "ParticleSystem.h"
#include "DrawableComponent.h"

namespace Sentinel
{
	class SENTINEL_DLL SpriteComponent : public DrawableComponent
	{
		friend class ParticleSystem;

		DECLARE_SERIAL( SpriteComponent );

	private:

		TransformComponent*		mTransform;

		/////////////////////////////////
		
		POINT					mSpriteSize;
		POINT					mSpriteDimension;

		UINT					mNumFrames;

		Texture*				mTexture;

		ColorRGBA				mColor;
		UINT					mFrame;

	public:

		SpriteComponent();
		SpriteComponent( Texture* texture, const POINT& spriteSize );

		void	Set( Texture* texture, const POINT& spriteSize );

		void	Startup();

		void	Update();

		void	Shutdown();

		/////////////////////////////////

		void	SetMaterial( const Material& material );

		const Material& GetMaterial();

		void	SetColor( const ColorRGBA& color );

		void	SetFrame( UINT frame );
	};
}
