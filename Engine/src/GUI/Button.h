#pragma once

#include "GUI/Widget.h"

namespace Sentinel { namespace GUI
{
	class SENTINEL_DLL Button : public Widget
	{
	public:

		UINT			mFrameUp;
		UINT			mFrameOver;
		UINT			mFrameDown;

		enum State
		{
			UP,
			OVER,
			DOWN,
		};

	protected:

		int				mState;

	public:

		FuncPtr0		ActionUp;
		FuncPtr0		ActionOver;
		FuncPtr0		ActionDown;
		FuncPtr0		ActionClick;

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
