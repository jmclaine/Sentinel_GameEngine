#pragma once

#include "PhysicsComponent.h"

namespace Sentinel
{
	class ControllerComponent : public GameComponent
	{
	protected:

		PhysicsComponent*		mPhysics;
		TransformComponent*		mTransform;

		//////////////////////////////

		ControllerComponent();

	public:

		void Startup();

		void Update();

		void Shutdown();
	};
}
