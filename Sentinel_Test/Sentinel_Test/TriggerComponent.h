#pragma once

#include "GameComponent.h"

namespace Sentinel
{
	class PhysicsComponent;

	class SENTINEL_DLL TriggerComponent : public GameComponent
	{
		DECLARE_SERIAL();

		PhysicsComponent*	mPhysics;

	public:

		TriggerComponent();
		~TriggerComponent();

		void Startup();

		void Update();

		void Shutdown();
	};
}
