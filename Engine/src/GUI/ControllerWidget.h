#pragma once

#include "GUI\Widget.h"

namespace Sentinel { namespace GUI
{
	class SENTINEL_DLL ControllerWidget : public Widget
	{
	public:

		bool			mIsActive;

	protected:

		bool			mIsOver;

	public:

		WidgetFunc		mActionOver;

	protected:

		ControllerWidget();

	public:

		virtual ~ControllerWidget();

		virtual void	SerialSave( Archive& archive ) = 0;

		void			Save( Archive& archive );
		void			Load( Archive& archive );
	};
}}
