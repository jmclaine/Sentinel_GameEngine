#include "GameObject.h"
#include "PhysicsComponent.h"
#include "Timing.h"

namespace Sentinel
{
	DEFINE_SERIAL_REGISTER( PhysicsComponent );
	DEFINE_SERIAL_CLONE( PhysicsComponent );

	PhysicsComponent::PhysicsComponent()
	{
		mType		= GameComponent::PHYSICS;

		mTransform	= NULL;
	}

	PhysicsComponent::PhysicsComponent( btRigidBody* body )
	{
		mType		= GameComponent::PHYSICS;

		mTransform	= NULL;
		mRigidBody	= body;
	}

	void PhysicsComponent::Startup()
	{
		mTransform = (TransformComponent*)mOwner->FindComponent( GameComponent::TRANSFORM );

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

	/////////////////////////////////

	btRigidBody* PhysicsComponent::GetRigidBody()
	{
		return mRigidBody;
	}

	void PhysicsComponent::SetRigidBody( btRigidBody* body )
	{
		// Remove old rigid body from PhysicsSystem.
		//
		if( !mRigidBody )
		{
			mRigidBody = body;
		}
		//else
		//{}
	}

	/////////////////////////////////

	void PhysicsComponent::Save( Archive& archive )
	{
		mSerialRegistry.Save( archive );

		int type = mRigidBody->getCollisionShape()->getShapeType();
		
		archive.Write( &type );

		float mass = 1.0f / mRigidBody->getInvMass();
		archive.Write( &mass );

		switch( type )
		{
			case SPHERE_SHAPE_PROXYTYPE:
			case BOX_SHAPE_PROXYTYPE:
			case CYLINDER_SHAPE_PROXYTYPE:
			case CONVEX_TRIANGLEMESH_SHAPE_PROXYTYPE:
				break;

			default:
				throw AppException( "No support for PhysicsComponent shape on " + std::string( mOwner->mName ));
				break;
		}
	}

	void PhysicsComponent::Load( Archive& archive )
	{
		int type;
		archive.Read( &type );
		
		float mass;
		archive.Read( &mass );
		
		Startup();

		switch( type )
		{
			case SPHERE_SHAPE_PROXYTYPE:
				mRigidBody = PhysicsSystem::Inst()->CreateSphere( mTransform->mPosition, mTransform->mOrientation, mTransform->mScale.x, mass );
				break;

			case BOX_SHAPE_PROXYTYPE:
				mRigidBody = PhysicsSystem::Inst()->CreateBox( mTransform->mPosition, mTransform->mOrientation, mTransform->mScale, mass );
				break;

			case CYLINDER_SHAPE_PROXYTYPE:
				mRigidBody = PhysicsSystem::Inst()->CreateCylinder( mTransform->mPosition, mTransform->mOrientation, mTransform->mScale, mass );
				break;

			case CONVEX_TRIANGLEMESH_SHAPE_PROXYTYPE:
				//mRigidBody = PhysicsSystem::Inst()->CreateMesh( mTransform->mPosition, mTransform->mOrientation, mTransform->mScale, mesh, mass );
				//break;

			default:
				throw AppException( "No support for PhysicsComponent shape on " + std::string( mOwner->mName ));
				break;
		}
	}
}
