#pragma once

#include "GUI/Button.h"

namespace Sentinel { namespace GUI
{
	class SENTINEL_DLL ButtonSprite : public Button
	{
		DECLARE_SERIAL();

	public:

		UINT			mFrameUp;
		ColorRGBA		mColorUp;

		UINT			mFrameOver;
		ColorRGBA		mColorOver;

		UINT			mFrameDown;
		ColorRGBA		mColorDown;

		//////////////////////////////////

		ButtonSprite();
		virtual ~ButtonSprite();

		virtual void	Update();
	};
}}
