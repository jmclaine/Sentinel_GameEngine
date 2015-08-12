#pragma once

#include "GameComponent.h"

namespace Sentinel {
namespace Component
{
	class Physics;

	class SENTINEL_DLL Trigger : public GameComponent
	{
		DECLARE_SERIAL();

		Physics* mPhysics;

	public:

		Trigger();
		~Trigger();

		void Startup();

		void Update();

		void Shutdown();
	};
}}
