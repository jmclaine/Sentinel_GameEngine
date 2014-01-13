#pragma once

#include "ControllerComponent.h"
#include "Input.h"

namespace Sentinel
{
	class EditorControllerComponent : public ControllerComponent
	{
	private:

		POINT	mLastMousePos;

		UINT	mLastWindowWidth;
		UINT	mLastWindowHeight;

	public:

		float	mForwardSpeed;
		float	mStrafeSpeed;
		float	mAngularSpeed;

		//////////////////////////////
		
		EditorControllerComponent();

		void	Startup();

		void	Update();

		void	Shutdown();
	};
}
