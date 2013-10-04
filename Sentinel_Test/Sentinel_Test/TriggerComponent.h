#pragma once

#include "PhysicsComponent.h"

namespace Sentinel
{
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
