#pragma once

#include "GameComponent.h"

namespace Sentinel
{
	class TransformComponent;
	class RigidBody;

	class SENTINEL_DLL PhysicsComponent : public GameComponent
	{
		DECLARE_SERIAL();
		
	protected:

		TransformComponent*		mTransform;

		RigidBody*				mRigidBody;

	public:

		PhysicsComponent();
		PhysicsComponent( RigidBody* body );
		~PhysicsComponent();

		void			Startup();

		void			Update();

		void			Shutdown();

		/////////////////////////////////

		RigidBody*		GetRigidBody();

		void			SetRigidBody( RigidBody* body );
	};
}
