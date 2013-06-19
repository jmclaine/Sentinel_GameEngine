#pragma once

#include <vector>

#include "MathLib.h"
#include "PhysicsSystem.h"
#include "TransformComponent.h"

namespace Sentinel
{
	class PhysicsComponent : public GameComponent
	{
		TransformComponent*	mTransform;

	public:

		btRigidBody*		mRigidBody;
		
		//////////////////////////////

		PhysicsComponent( btRigidBody* body );
		
		void Startup();

		void Update();
		
		void Shutdown();
	};
}
