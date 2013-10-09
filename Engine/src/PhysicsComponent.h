#pragma once
/*
Bullet Physics is not compatible with the
managed / wrapped version due to byte-alignment.
*/
#ifndef IGNORE_PHYSICS
#include "PhysicsSystem.h"
#endif

#include "TransformComponent.h"

namespace Sentinel
{
	enum ShapeType
	{
		SHAPE_INVALID,
		SHAPE_SPHERE,
		SHAPE_BOX,
		SHAPE_CYLINDER,
		SHAPE_MESH,
	};

	enum PhysicsFlag
	{
		DISABLE_GRAVITY			= 1,
		ENABLE_GYROSCOPIC_FORCE = 2,
	};

	class SENTINEL_DLL PhysicsComponent : public GameComponent
	{
	protected:

		DECLARE_SERIAL();

		TransformComponent*	mTransform;

#ifndef IGNORE_PHYSICS
		btRigidBody*		mRigidBody;
#endif

	public:

		PhysicsComponent();

#ifndef IGNORE_PHYSICS
		PhysicsComponent( btRigidBody* body );
#endif
		~PhysicsComponent();

		void			Startup();

		void			Update();
		
		void			Shutdown();

		/////////////////////////////////

#ifndef IGNORE_PHYSICS
		btRigidBody*	GetRigidBody();
		void			SetRigidBody( btRigidBody* body );
#endif
	};
}
