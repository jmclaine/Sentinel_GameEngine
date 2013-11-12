#pragma once
/*
Render a single quad sprite.
*/
#include <memory>

#include "ColorRGBA.h"
#include "DrawableComponent.h"
#include "SpriteSystem.h"

namespace Sentinel
{
	class TransformComponent;
	class CameraComponent;
	class Sprite;

	class SENTINEL_DLL SpriteComponent : public DrawableComponent
	{
		DECLARE_SERIAL();

	protected:

		TransformComponent*		mTransform;

		SpriteSystem*			mSpriteSystem;

		CameraComponent*		mCamera;

	public:

		UINT					mFrame;
		ColorRGBA				mColor;

	protected:

		SpriteComponent();

	public:

		SpriteComponent( SpriteSystem* spriteSystem, CameraComponent* camera );
		~SpriteComponent();

		void	Set( SpriteSystem* spriteSystem, CameraComponent* camera );

		/////////////////////////////////

		void	Startup();

		void	Update();

		void	Shutdown();
	};
}
