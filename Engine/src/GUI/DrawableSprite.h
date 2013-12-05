#pragma once

#include "GUI/Widget.h"

namespace Sentinel { namespace GUI
{
	class SENTINEL_DLL DrawableSprite : public Widget
	{
		DECLARE_SERIAL();

	public:

		UINT mFrame;

		//////////////////////////////////

		DrawableSprite();
		~DrawableSprite();

		void Update();
	};
}}
