#pragma once

#include "GameComponent.h"

namespace Sentinel {
namespace Component
{
	class Physics;
	class Transform;

	class SENTINEL_DLL Controller : public GameComponent
	{
	protected:

		Physics* mPhysics;
		Transform* mTransform;

		//////////////////////////////

		Controller();

	public:

		virtual void Startup();

		virtual void Update();

		virtual void Shutdown();

		//////////////////////////////

		virtual void Execute();

		virtual void SetOwner(GameObject* owner);
	};
}}
