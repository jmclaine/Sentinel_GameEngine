#pragma once

#include "GUI\ControllerWidget.h"
#include "Vector3f.h"
#include "Point.h"

namespace Sentinel { namespace GUI
{
	class SENTINEL_DLL SpriteControllerWidget : public ControllerWidget
	{
		DECLARE_SERIAL();

	public:

		DECLARE_SERIAL_REGISTER_SAVE();

	protected:

		Vector3f		mBasePosition;
		Vector3f		mBaseScale;

	public:

		bool			mPositionToWindowX;
		bool			mPositionToWindowY;

		bool			mScaleToWindowX;
		bool			mScaleToWindowY;

		Quad			mMargin;

	public:

		SpriteControllerWidget();
		virtual ~SpriteControllerWidget();

		///////////////////////////////////

		virtual void Startup();

		virtual void Update();

		virtual void Shutdown();
	};
}}
