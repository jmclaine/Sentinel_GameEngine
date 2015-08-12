#pragma once

#include "Component/Camera.h"

namespace Sentinel {
namespace Component
{
	class SENTINEL_DLL OrthographicCamera : public Camera
	{
		DECLARE_SERIAL();

	public:

		OrthographicCamera();
		OrthographicCamera(UINT windowWidth, UINT windowHeight);

		//////////////////////////////

		void Set(UINT windowWidth, UINT windowHeight);

		Ray ScreenPointToRay(UINT mouseX, UINT mouseY, UINT screenWidth = 0, UINT screenHeight = 0);

		//////////////////////////////

		GameComponent* Copy();
	};
}}
