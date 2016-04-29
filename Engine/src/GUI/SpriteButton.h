#pragma once

#include "GUI/SpriteController.h"
#include "GUI/Button.h"
#include "ColorRGBA.h"

namespace Sentinel
{
	namespace Component
	{
		class SpriteDrawable;
	}

namespace GUI
{
	class SENTINEL_DLL SpriteButton : public SpriteController, public Button
	{
		DECLARE_SERIAL();

	public:
		DECLARE_SERIAL_REGISTER_SAVE();

		UINT mFrameUp;
		ColorRGBA mColorUp;

		UINT mFrameOver;
		ColorRGBA mColorOver;

		UINT mFrameDown;
		ColorRGBA mColorDown;

	protected:
		Component::SpriteDrawable* mSprite;

	public:
		SpriteButton();
		virtual ~SpriteButton();

		virtual void Startup();

		virtual void Update();

		virtual void Shutdown();

		///////////////////////////////////

		GameComponent* Copy();
	};
}}
