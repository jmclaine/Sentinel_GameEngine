#pragma once

#include "Util.h"
#include "ColorRGBA.h"
#include "DrawableComponent.h"

namespace Sentinel
{
	class TransformComponent;
	class ParticleSystem;
	class Texture;
	class Material;
	class ColorRGBA;

	class SENTINEL_DLL SpriteComponent : public DrawableComponent
	{
		friend class ParticleSystem;

		DECLARE_SERIAL();

	private:

		TransformComponent*		mTransform;

		ParticleSystem*			mParticle;

		/////////////////////////////////
		
		POINT					mSpriteSize;
		POINT					mSpriteDimension;

		UINT					mNumFrames;

		std::shared_ptr< Texture >	mTexture;

		ColorRGBA				mColor;
		UINT					mFrame;

	public:

		SpriteComponent();
		SpriteComponent( ParticleSystem* particle, std::shared_ptr< Texture > texture, const POINT& spriteSize );

		void	Set( ParticleSystem* particle, std::shared_ptr< Texture > texture, const POINT& spriteSize );

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
