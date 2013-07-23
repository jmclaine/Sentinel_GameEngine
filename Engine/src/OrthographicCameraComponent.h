#pragma once

#include "CameraComponent.h"

namespace Sentinel
{
	class OrthographicCameraComponent : public CameraComponent
	{
	public:

		OrthographicCameraComponent( float windowWidth, float windowHeight );

		void Startup();

		void Update();

		void Shutdown();
	};
}
