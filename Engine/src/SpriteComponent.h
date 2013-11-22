#pragma once
/*
Render a single quad sprite.
*/
#include <memory>

#include "ColorRGBA.h"
#include "DrawableComponent.h"

namespace Sentinel
{
	class SpriteSystem;
	class TransformComponent;
	class CameraComponent;
	class Sprite;

	class SENTINEL_DLL SpriteComponent : public DrawableComponent
	{
		DECLARE_SERIAL();

	protected:

		SpriteSystem*			mSpriteSystem;

		UINT					mCamera;

	public:

		UINT					mFrame;
		ColorRGBA				mColor;

		std::shared_ptr< Sprite > mSprite;

	protected:

		SpriteComponent();

	public:

		SpriteComponent( std::shared_ptr< Sprite > sprite, UINT camera );
		~SpriteComponent();

		void	Set( std::shared_ptr< Sprite > sprite, UINT camera );

		/////////////////////////////////

		void	Startup();

		void	Update();

		void	Shutdown();
	};
}
