#pragma once

#include "Common.h"
#include "PhysicsComponent.h"

namespace Sentinel
{
	class SENTINEL_DLL ControllerComponent : public GameComponent
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
