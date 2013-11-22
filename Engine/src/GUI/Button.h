#pragma once

#include "GUI/Widget.h"

namespace Sentinel { namespace GUI
{
	class SENTINEL_DLL Button : public Widget
	{
		DECLARE_SERIAL();

	public:

		UINT			mFrameUp;
		UINT			mFrameOver;
		UINT			mFrameDown;

	protected:

		enum State
		{
			UP,
			OVER,
			DOWN,
		};

		State			mState;

	public:

		WidgetFunc		mActionUp;
		WidgetFunc		mActionDown;
		WidgetFunc		mActionClick;

		//////////////////////////////////

		Button();
		virtual ~Button();

		virtual void	Update();

	protected:

		virtual void	Up();
		virtual void	Over();
		virtual void	Down();
		virtual void	Click();
	};
}}
