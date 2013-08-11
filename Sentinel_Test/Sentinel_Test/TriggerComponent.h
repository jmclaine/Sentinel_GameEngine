#pragma once

#include "Common.h"
#include "PhysicsComponent.h"

namespace Sentinel
{
	class SENTINEL_DLL TriggerComponent : public GameComponent
	{
		PhysicsComponent*	mPhysics;

	public:

		TriggerComponent();
		~TriggerComponent();

		void Startup();

		void Update();

		void Shutdown();
	};
}
