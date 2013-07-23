#pragma once

#include <vector>

#include "PhysicsSystem.h"
#include "TransformComponent.h"

namespace Sentinel
{
	class PhysicsComponent : public GameComponent
	{
		TransformComponent*	mTransform;

		btRigidBody*		mRigidBody;

	public:

		PhysicsComponent( btRigidBody* body );

		btRigidBody* GetRigidBody();
		
		void Startup();

		void Update();
		
		void Shutdown();
	};
}
