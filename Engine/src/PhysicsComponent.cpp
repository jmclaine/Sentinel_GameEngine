#include "GameObject.h"
#include "PhysicsComponent.h"
#include "Timing.h"

namespace Sentinel
{
	PhysicsComponent::PhysicsComponent( btRigidBody* body )
	{
		mComponentType	= COMPONENT_PHYSICS;

		mTransform		= NULL;
		mRigidBody		= body;
	}

	void PhysicsComponent::Startup()
	{
		mTransform = (TransformComponent*)mOwner->FindComponent( COMPONENT_TRANSFORM );

		if( mTransform == NULL )
			throw AppException( "PhysicsComponent::Startup()\n" + std::string( mOwner->mName ) + " does not contain TransformComponent" );
	}

	void PhysicsComponent::Update()
	{
		btVector3 v( mRigidBody->getCenterOfMassPosition() );

		mTransform->mPosition = Vector3f( v.x(), v.y(), v.z() );
		
		btQuaternion q( mRigidBody->getOrientation() );

		mTransform->mOrientation.x = q.x();
		mTransform->mOrientation.y = q.y();
		mTransform->mOrientation.z = q.z();
		mTransform->mOrientation.w = q.w();
	}

	void PhysicsComponent::Shutdown()
	{
		mTransform = NULL;
	}

	////////////////////////

	btRigidBody* PhysicsComponent::GetRigidBody()
	{
		return mRigidBody;
	}
}
