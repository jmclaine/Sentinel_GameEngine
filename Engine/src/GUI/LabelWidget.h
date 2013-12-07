#pragma once

#include "GUI/ViewWidget.h"
#include "ColorRGBA.h"

namespace Sentinel { namespace GUI
{
	class SENTINEL_DLL LabelWidget : public ViewWidget
	{
		DECLARE_SERIAL();

	public:

		DECLARE_SERIAL_REGISTER_SAVE();

		std::string		mText;
		ColorRGBA		mColor;

		//////////////////////////////////

		LabelWidget();
		LabelWidget( const std::string& text );
		virtual ~LabelWidget();

		//////////////////////////////////

		virtual void	Startup();

		virtual void	Update();

		virtual void	Shutdown();
	};
}}
