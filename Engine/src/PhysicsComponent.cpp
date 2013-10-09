#include "GameObject.h"
#include "PhysicsComponent.h"
#include "Timing.h"

namespace Sentinel
{
#ifndef IGNORE_PHYSICS
	ShapeType ToSentinelShapeType( int type )
	{
		switch( type )
		{
			case SPHERE_SHAPE_PROXYTYPE:
				return SHAPE_SPHERE;

			case BOX_SHAPE_PROXYTYPE:
				return SHAPE_BOX;

			case CYLINDER_SHAPE_PROXYTYPE:
				return SHAPE_CYLINDER;

			case CONVEX_TRIANGLEMESH_SHAPE_PROXYTYPE:
				return SHAPE_MESH;

			default:
				return SHAPE_INVALID;
		}
	}
#endif

	DEFINE_SERIAL_REGISTER( PhysicsComponent );
	DEFINE_SERIAL_CLONE( PhysicsComponent );

	PhysicsComponent::PhysicsComponent()
	{
		mType = GameComponent::PHYSICS;

		mTransform = NULL;
	}

#ifndef IGNORE_PHYSICS
	PhysicsComponent::PhysicsComponent( btRigidBody* body )
	{
		mType = GameComponent::PHYSICS;

		mTransform = NULL;

		mRigidBody = body;
	}
#endif

	PhysicsComponent::~PhysicsComponent()
	{
#ifndef IGNORE_PHYSICS
		if( mRigidBody )
		{
			PhysicsSystem::Inst()->RemoveRigidBody( mRigidBody );

			delete mRigidBody->getMotionState();
			delete mRigidBody;

			mRigidBody = NULL;
		}
#endif
	}

	void PhysicsComponent::Startup()
	{
		mTransform = (TransformComponent*)mOwner->FindComponent( GameComponent::TRANSFORM );

		if( mTransform == NULL )
			throw AppException( "PhysicsComponent::Startup()\n" + std::string( mOwner->mName ) + " does not contain TransformComponent" );

#ifndef IGNORE_PHYSICS
		if( mRigidBody == NULL )
			throw AppException( "PhysicsComponent::Startup()\n" + std::string( mOwner->mName ) + " does not contain RigidBody" );

		PhysicsSystem::Inst()->AddRigidBody( mRigidBody );
#endif
	}

	void PhysicsComponent::Update()
	{
#ifndef IGNORE_PHYSICS
		if( mTransform )
		{
			btVector3 v( mRigidBody->getCenterOfMassPosition() );

			mTransform->mPosition = Vector3f( v.x(), v.y(), v.z() );
		
			btQuaternion q( mRigidBody->getOrientation() );

			mTransform->mOrientation = Quatf( q.x(), q.y(), q.z(), q.w() );
		}
#endif
	}

	void PhysicsComponent::Shutdown()
	{
		mTransform = NULL;

#ifndef IGNORE_PHYSICS
		PhysicsSystem::Inst()->RemoveRigidBody( mRigidBody );
#endif
	}

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

	/////////////////////////////////

	void PhysicsComponent::Save( Archive& archive )
	{
#ifndef IGNORE_PHYSICS
		mSerialRegistry.Save( archive );

		GameComponent::Save( archive );

		int type = (int)ToSentinelShapeType( mRigidBody->getCollisionShape()->getShapeType() );
		
		if( type == SHAPE_INVALID )
			REPORT_ERROR( "No support for PhysicsComponent shape on " << mOwner->mName, "Save Error" );

		archive.Write( &type );

		float mass = 1.0f / mRigidBody->getInvMass();
		archive.Write( &mass );

		btVector3 pos = mRigidBody->getCenterOfMassPosition();
		archive.Write( &pos.x() );
		archive.Write( &pos.y() );
		archive.Write( &pos.z() );

		btQuaternion rot = mRigidBody->getOrientation();
		archive.Write( &rot.x() );
		archive.Write( &rot.y() );
		archive.Write( &rot.z() );
		archive.Write( &rot.w() );

		btVector3 scale = mRigidBody->getCollisionShape()->getLocalScaling();
		archive.Write( &scale.x() );
		archive.Write( &scale.y() );
		archive.Write( &scale.z() );

		int flags = mRigidBody->getFlags();
		archive.Write( &flags );

		float value = mRigidBody->getLinearDamping();
		archive.Write( &value );

		value = mRigidBody->getAngularDamping();
		archive.Write( &value );

		value = mRigidBody->getRestitution();
		archive.Write( &value );

		value = mRigidBody->getFriction();
		archive.Write( &value );

		btVector3 v = mRigidBody->getAngularFactor();
		archive.Write( &v.x() );
		archive.Write( &v.y() );
		archive.Write( &v.z() );

		v = mRigidBody->getGravity();
		archive.Write( &v.x() );
		archive.Write( &v.y() );
		archive.Write( &v.z() );
#endif
	}

	void PhysicsComponent::Load( Archive& archive )
	{
#ifndef IGNORE_PHYSICS
		GameComponent::Load( archive );

		int type;
		archive.Read( &type );
		
		float mass;
		archive.Read( &mass );

		Vector3f pos;
		Quatf    rot;
		Vector3f scale;

		archive.Read( pos.Ptr(),	ar_sizeof( pos ));
		archive.Read( rot.Ptr(),	ar_sizeof( rot ));
		archive.Read( scale.Ptr(),	ar_sizeof( scale ));
	
		switch( type )
		{
			case SHAPE_SPHERE:
				mRigidBody = PhysicsSystem::Inst()->CreateSphere( pos, rot, scale.x, mass );
				break;

			case SHAPE_BOX:
				mRigidBody = PhysicsSystem::Inst()->CreateBox( pos, rot, scale, mass );
				break;

			case SHAPE_CYLINDER:
				mRigidBody = PhysicsSystem::Inst()->CreateCylinder( pos, rot, scale, mass );
				break;

			case SHAPE_MESH:
				//mRigidBody = PhysicsSystem::Inst()->CreateMesh( pos, rot, scale, mesh, mass );
				//break;

			default:
				REPORT_ERROR( "No support for PhysicsComponent shape on " << mOwner->mName, "Save Error" );
				break;
		}

		int flags;
		archive.Read( &flags );
		mRigidBody->setFlags( flags );
		
		float linearDamping;
		archive.Read( &linearDamping );
		float angularDamping;
		archive.Read( &angularDamping );
		mRigidBody->setDamping( linearDamping, angularDamping );
		
		float rest;
		archive.Read( &rest );
		mRigidBody->setRestitution( rest );

		float friction;
		archive.Read( &friction );
		mRigidBody->setFriction( friction );

		Vector3f angularFactor;
		archive.Read( angularFactor.Ptr(), ar_sizeof( angularFactor ));
		mRigidBody->setAngularFactor( btVector3( angularFactor.x, angularFactor.y, angularFactor.z ));
		
		Vector3f gravity;
		archive.Read( gravity.Ptr(), ar_sizeof( gravity ));
		mRigidBody->setGravity( btVector3( gravity.x, gravity.y, gravity.z ));
#endif
	}
}
