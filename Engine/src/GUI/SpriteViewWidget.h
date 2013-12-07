#pragma once

#include "GUI\ViewWidget.h"
#include "ColorRGBA.h"

namespace Sentinel { namespace GUI
{
	class SENTINEL_DLL SpriteViewWidget : public ViewWidget
	{
		DECLARE_SERIAL();

	public:

		DECLARE_SERIAL_REGISTER_SAVE();

		UINT			mFrame;
		ColorRGBA		mColor;

		//////////////////////////

		SpriteViewWidget();
		virtual ~SpriteViewWidget();

		//////////////////////////

		virtual void	Startup();

		virtual void	Update();

		virtual void	Shutdown();
	};
}}
