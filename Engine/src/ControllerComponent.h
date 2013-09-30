#pragma once

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

		virtual void Startup();

		virtual void Update();

		virtual void Shutdown();
	};
}
