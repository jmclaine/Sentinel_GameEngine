#pragma once

#include "ControllerComponent.h"

namespace Sentinel
{
	class SENTINEL_DLL PlayerControllerComponent : public ControllerComponent
	{
		DECLARE_SERIAL( PlayerControllerComponent );

	public:

		float mSpeed;
		float mAngularSpeed;

		//////////////////////////////
		
		PlayerControllerComponent();

		void Startup();

		void Update();

		void Shutdown();
	};
}
