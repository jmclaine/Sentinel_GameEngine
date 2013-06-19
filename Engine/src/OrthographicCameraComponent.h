#pragma once

#include "CameraComponent.h"

namespace Sentinel
{
	class OrthographicCameraComponent : public CameraComponent
	{
	public:

		OrthographicCameraComponent( HWND hWnd );

		void Startup();

		void Update();

		void Shutdown();
	};
}
