#pragma once

#include "ControllerComponent.h"
#include "GUI\ModelWidget.h"
#include "Input.h"

namespace Sentinel
{
	class EditorControllerComponent : public ControllerComponent
	{
	private:

		POINT	mLastMousePos;

		float	mLastWindowWidth;
		float	mLastWindowHeight;

	public:

		float	mForwardSpeed;
		float	mStrafeSpeed;
		float	mAngularSpeed;

		GUI::ModelWidget* mWorldWidget;

		//////////////////////////////
		
		EditorControllerComponent();

		void	Startup();

		void	Update();

		void	Shutdown();
	};
}
