#pragma once

#include "CameraComponent.h"

namespace Sentinel
{
	class SENTINEL_DLL OrthographicCameraComponent : public CameraComponent
	{
		DECLARE_SERIAL();

	public:

		OrthographicCameraComponent();
		OrthographicCameraComponent( float windowWidth, float windowHeight );

		void Startup();

		void Update();

		void Shutdown();

		//////////////////////////////

		void Set( float windowWidth, float windowHeight );

		Ray	 ScreenPointToRay( UINT mouseX, UINT mouseY, UINT screenWidth = 0, UINT screenHeight = 0 );
	};
}
