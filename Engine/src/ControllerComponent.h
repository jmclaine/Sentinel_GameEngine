#pragma once

#include "GameComponent.h"

namespace Sentinel
{
	class PhysicsComponent;
	class TransformComponent;

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
