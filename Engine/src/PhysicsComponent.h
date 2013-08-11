#pragma once

#include <vector>

#include "Common.h"
#include "PhysicsSystem.h"
#include "TransformComponent.h"

namespace Sentinel
{
	class SENTINEL_DLL PhysicsComponent : public GameComponent
	{
		TransformComponent*	mTransform;

		btRigidBody*		mRigidBody;

	public:

		PhysicsComponent( btRigidBody* body );

		void Startup();

		void Update();
		
		void Shutdown();

		////////////////////////

		btRigidBody* GetRigidBody();
	};
}
