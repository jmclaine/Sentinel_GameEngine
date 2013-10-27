#pragma once
/*
Render a single quad sprite.
*/
#include <memory>

#include "ColorRGBA.h"
#include "DrawableComponent.h"

namespace Sentinel
{
	class TransformComponent;
	class ParticleSystem;
	class Sprite;

	class SENTINEL_DLL SpriteComponent : public DrawableComponent
	{
		DECLARE_SERIAL();

	protected:

		TransformComponent*		mTransform;

		ParticleSystem*			mParticle;

	public:
		
		std::shared_ptr< Sprite > mSprite;

	protected:

		SpriteComponent();

	public:

		SpriteComponent( ParticleSystem* particle, std::shared_ptr< Sprite > sprite );

		void	Set( ParticleSystem* particle, std::shared_ptr< Sprite > sprite );

		/////////////////////////////////

		void	Startup();

		void	Update();

		void	Shutdown();
	};
}
