#pragma once

#include "GUI\ControllerWidget.h"

namespace Sentinel { namespace GUI
{
	class SENTINEL_DLL SpriteControllerWidget : public ControllerWidget
	{
		DECLARE_SERIAL();

	public:

		DECLARE_SERIAL_REGISTER_SAVE();

		SpriteControllerWidget();
		virtual ~SpriteControllerWidget();

		///////////////////////////////////

		virtual void Startup();

		virtual void Update();

		virtual void Shutdown();
	};
}}
