#pragma once

#include "GUI/Widget.h"

namespace Sentinel { namespace GUI
{
	class SENTINEL_DLL Button : public Widget
	{
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

	protected:

		Button();

	public:

		virtual ~Button();

	protected:

		virtual void	PreUpdate();

	public:

		virtual void	Update() = 0;

	protected:

		virtual void	PostUpdate();

	public:

		virtual void	Save( Archive& archive );

		virtual void	Load( Archive& archive );
	};
}}
