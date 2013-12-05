#pragma once
/*
Render a single quad sprite.
*/
#include <memory>

#include "ColorRGBA.h"
#include "DrawableComponent.h"
#include "GUI/Widget.h"

namespace Sentinel
{
	class SpriteSystem;
	class TransformComponent;
	class CameraComponent;
	class Sprite;

	class SENTINEL_DLL WidgetComponent : public DrawableComponent
	{
		DECLARE_SERIAL();

	protected:

		SpriteSystem*			mSpriteSystem;
		FontSystem*				mFontSystem;

		UINT					mCamera;

	public:

		GUI::Root				mRoot;

		std::shared_ptr< Sprite > mSprite;

	protected:

		WidgetComponent();

	public:

		WidgetComponent( std::shared_ptr< Sprite > sprite, FontSystem* fontSystem, UINT camera );
		~WidgetComponent();

		void	Set( std::shared_ptr< Sprite > sprite, FontSystem* fontSystem, UINT camera );

		/////////////////////////////////

		void	Startup();

		void	Update();

		void	Shutdown();
	};
}
