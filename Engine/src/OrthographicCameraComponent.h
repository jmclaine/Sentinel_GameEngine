#pragma once

#include "Common.h"
#include "CameraComponent.h"

namespace Sentinel
{
	class SENTINEL_DLL OrthographicCameraComponent : public CameraComponent
	{
	public:

		OrthographicCameraComponent( float windowWidth, float windowHeight );

		void Startup();

		void Update();

		void Shutdown();

		//////////////////////////////

		void Set( float windowWidth, float windowHeight );
	};
}
