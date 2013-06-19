#pragma once

#include "ControllerComponent.h"

namespace Sentinel
{
	class PlayerControllerComponent : public ControllerComponent
	{
	public:

		float mSpeed;
		float mAngularSpeed;

		//////////////////////////////
		
		PlayerControllerComponent();

		void Update();
	};
}
