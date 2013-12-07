#pragma once

#include "GUI/SpriteControllerWidget.h"
#include "GUI/Button.h"
#include "ColorRGBA.h"

namespace Sentinel { namespace GUI
{
	class SENTINEL_DLL SpriteButtonWidget : public SpriteControllerWidget, public Button
	{
		DECLARE_SERIAL();

	public:

		DECLARE_SERIAL_REGISTER_SAVE();

		UINT			mFrameUp;
		ColorRGBA		mColorUp;

		UINT			mFrameOver;
		ColorRGBA		mColorOver;

		UINT			mFrameDown;
		ColorRGBA		mColorDown;

		//////////////////////////////////

		SpriteButtonWidget();
		virtual ~SpriteButtonWidget();

		virtual void	Startup();

		virtual void	Update();

		virtual void	Shutdown();
	};
}}
