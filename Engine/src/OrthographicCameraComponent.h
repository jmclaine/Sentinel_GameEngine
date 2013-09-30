#pragma once

#include "CameraComponent.h"

namespace Sentinel
{
	class SENTINEL_DLL OrthographicCameraComponent : public CameraComponent
	{
		DECLARE_SERIAL( OrthographicCameraComponent );

	public:

		OrthographicCameraComponent();
		OrthographicCameraComponent( float windowWidth, float windowHeight );

		void Startup();

		void Update();

		void Shutdown();

		//////////////////////////////

		void Set( float windowWidth, float windowHeight );
	};
}
