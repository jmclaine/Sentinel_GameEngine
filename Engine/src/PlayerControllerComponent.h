#pragma once

#include "Common.h"
#include "ControllerComponent.h"

namespace Sentinel
{
	class SENTINEL_DLL PlayerControllerComponent : public ControllerComponent
	{
	public:

		float mSpeed;
		float mAngularSpeed;

		//////////////////////////////
		
		PlayerControllerComponent();

		void Update();
	};
}
