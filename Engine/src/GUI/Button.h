#pragma once

#include "GUI/Widget.h"

namespace Sentinel { namespace GUI
{
	class SENTINEL_DLL Button
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

		////////////////////////////////

		virtual void	Update( bool isOver );

		////////////////////////////////

		virtual void	Save( Archive& archive );
		virtual void	Load( Archive& archive );
	};
}}
