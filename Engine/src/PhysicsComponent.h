#pragma once

#include <vector>

#include "PhysicsSystem.h"
#include "TransformComponent.h"

namespace Sentinel
{
	class SENTINEL_DLL PhysicsComponent : public GameComponent
	{
		DECLARE_SERIAL();

		TransformComponent*	mTransform;

		btRigidBody*		mRigidBody;

	public:

		PhysicsComponent();
		PhysicsComponent( btRigidBody* body );

		void			Startup();

		void			Update();
		
		void			Shutdown();

		/////////////////////////////////

		btRigidBody*	GetRigidBody();
		void			SetRigidBody( btRigidBody* body );
	};
}
