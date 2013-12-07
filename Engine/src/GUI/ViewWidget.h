#pragma once

#include "GUI\Widget.h"

namespace Sentinel { namespace GUI
{
	class SENTINEL_DLL ViewWidget : public Widget
	{
	public:

		virtual DECLARE_SERIAL_REGISTER_SAVE() = 0;

	public:

		bool	mIsVisible;

	protected:

		ViewWidget();

	public:

		virtual ~ViewWidget();

		void	Save( Archive& archive );
		void	Load( Archive& archive );
	};
}}
