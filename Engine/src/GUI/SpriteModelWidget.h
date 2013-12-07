#pragma once

#include "GUI\ModelWidget.h"
#include "Vector3f.h"
#include "Point.h"

namespace Sentinel { namespace GUI
{
	class SENTINEL_DLL SpriteModelWidget : public ModelWidget
	{
		DECLARE_SERIAL();

	public:

		DECLARE_SERIAL_REGISTER_SAVE();

	private:

		bool			mDidUpdatePosition;
		bool			mDidUpdateScale;

	protected:

		Vector3f		mRelativePosition;
		Vector3f		mRelativeScale;

	public:

		bool			mPositionToWindowX;
		bool			mPositionToWindowY;

		bool			mScaleToWindowX;
		bool			mScaleToWindowY;

		Quad			mMargin;

	public:

		SpriteModelWidget();
		virtual ~SpriteModelWidget();

		///////////////////////////////////

		virtual void	Startup();

		virtual void	Update();

		virtual void	Shutdown();

		///////////////////////////////////

		void			UpdateRelativePosition();
		const Vector3f& GetRelativePosition();

		void			UpdateRelativeScale();
		const Vector3f& GetRelativeScale();
	};
}}
