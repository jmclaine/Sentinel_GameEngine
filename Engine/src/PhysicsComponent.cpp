#include "PhysicsComponent.h"
#include "MeshManager.h"
#include "GameObject.h"

namespace Sentinel
{
	DEFINE_SERIAL_REGISTER( PhysicsComponent );
	DEFINE_SERIAL_CLONE( PhysicsComponent );

	/////////////////////////////////

#ifndef IGNORE_PHYSICS
	btRigidBody* PhysicsComponent::GetRigidBody()
	{
		return mRigidBody;
	}

	void PhysicsComponent::SetRigidBody( btRigidBody* body )
	{
		// Remove old rigid body from PhysicsSystem.
		//
		if( mRigidBody )
			PhysicsSystem::Inst()->RemoveRigidBody( mRigidBody );

		mRigidBody = body;
	}
#endif

}
