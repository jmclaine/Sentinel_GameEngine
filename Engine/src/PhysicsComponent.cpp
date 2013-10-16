#include "PhysicsComponent.h"
#include "TransformComponent.h"
#include "PhysicsSystem.h"
#include "MeshManager.h"
#include "GameObject.h"
#include "GameWorld.h"
#include "Archive.h"
#include "Vector3f.h"
#include "Quatf.h"

namespace Sentinel
{
	DEFINE_SERIAL_REGISTER( PhysicsComponent );
	DEFINE_SERIAL_CLONE( PhysicsComponent );

	/////////////////////////////////

	PhysicsComponent::PhysicsComponent()
	{
		mType = GameComponent::PHYSICS;

		mTransform = NULL;

		mRigidBody = NULL;
	}

	PhysicsComponent::PhysicsComponent( RigidBody* body )
	{
		mType = GameComponent::PHYSICS;

		mTransform = NULL;

		mRigidBody = body;
	}

	PhysicsComponent::~PhysicsComponent()
	{
		SAFE_DELETE( mRigidBody );
	}

	void PhysicsComponent::Startup()
	{
		_ASSERT( mOwner );
		_ASSERT( mOwner->World() );
		_ASSERT( mOwner->World()->mPhysicsSystem );

		mTransform = (TransformComponent*)mOwner->FindComponent( GameComponent::TRANSFORM );

		if( mTransform == NULL )
			throw AppException( "PhysicsComponent::Startup()\n" + std::string( mOwner->mName ) + " does not contain TransformComponent" );

		if( mRigidBody == NULL )
			throw AppException( "PhysicsComponent::Startup()\n" + std::string( mOwner->mName ) + " does not contain RigidBody" );

		mOwner->World()->mPhysicsSystem->AddRigidBody( mRigidBody );
	}

	void PhysicsComponent::Update()
	{
		if( mTransform )
		{
			mTransform->mPosition    = mRigidBody->GetPosition();
			mTransform->mOrientation = mRigidBody->GetOrientation();
		}
	}

	void PhysicsComponent::Shutdown()
	{
		mTransform = NULL;

		mOwner->World()->mPhysicsSystem->RemoveRigidBody( mRigidBody );
	}

	/////////////////////////////////

	RigidBody* PhysicsComponent::GetRigidBody()
	{
		return mRigidBody;
	}

	void PhysicsComponent::SetRigidBody( RigidBody* body )
	{
		_ASSERT( body );

		// Remove old rigid body from PhysicsSystem.
		//
		if( mRigidBody )
			mOwner->World()->mPhysicsSystem->RemoveRigidBody( mRigidBody );

		mRigidBody = body;

		mOwner->World()->mPhysicsSystem->AddRigidBody( mRigidBody );
	}

	void PhysicsComponent::Save( Archive& archive )
	{
		mSerialRegistry.Save( archive );

		GameComponent::Save( archive );

		int type = (int)mRigidBody->GetShapeType();
		
		if( type == PHYSICS_INVALID )
			REPORT_ERROR( "No support for PhysicsComponent shape on " << mOwner->mName, "Save Error" );

		archive.Write( &type );

		float mass = mRigidBody->GetMass();
		archive.Write( &mass );

		Vector3f pos = mRigidBody->GetPosition();
		archive.Write( pos.Ptr(), ar_sizeof( pos ));
			
		Quatf rot = mRigidBody->GetOrientation();
		archive.Write( rot.Ptr(), ar_sizeof( rot ));
			
		Vector3f scale = mRigidBody->GetScale();
		archive.Write( scale.Ptr(), ar_sizeof( scale ));
			
		//if( type == RigidBody::MESH )

		int flags = mRigidBody->GetFlags();
		archive.Write( &flags );

		float value = mRigidBody->GetLinearDamping();
		archive.Write( &value );

		value = mRigidBody->GetAngularDamping();
		archive.Write( &value );

		value = mRigidBody->GetRestitution();
		archive.Write( &value );

		value = mRigidBody->GetFriction();
		archive.Write( &value );

		Vector3f v = mRigidBody->GetAngularFactor();
		archive.Write( v.Ptr(), ar_sizeof( v ));
			
		v = mRigidBody->GetGravity();
		archive.Write( v.Ptr(), ar_sizeof( v ));
	}

	void PhysicsComponent::Load( Archive& archive )
	{
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
	
		PhysicsSystem* physics;
		physics = mOwner->World()->mPhysicsSystem;

		switch( type )
		{
			case PHYSICS_SPHERE:
				mRigidBody = physics->CreateSphere( pos, rot, scale.x, mass );
				break;

			case PHYSICS_BOX:
				mRigidBody = physics->CreateBox( pos, rot, scale, mass );
				break;

			case PHYSICS_CYLINDER:
				mRigidBody = physics->CreateCylinder( pos, rot, scale, mass );
				break;

			case PHYSICS_MESH:
				/*{
					std::string name;
					archive.Read( &name );

					std::shared_ptr< Mesh > mesh = MeshManager::Inst()->Get( name );

					if( mesh )
						mRigidBody = physics->CreateMesh( pos, rot, scale, mesh.get(), mass );
					else
						REPORT_ERROR( "No support for PhysicsComponent shape on " << mOwner->mName, "Load Error" );
				}
				break;*/

			default:
				REPORT_ERROR( "No support for PhysicsComponent shape on " << mOwner->mName, "Load Error" );
				break;
		}

		int flags;
		archive.Read( &flags );
		mRigidBody->SetFlags( flags );
		
		float linearDamping;
		archive.Read( &linearDamping );
		float angularDamping;
		archive.Read( &angularDamping );
		mRigidBody->SetDamping( linearDamping, angularDamping );
		
		float rest;
		archive.Read( &rest );
		mRigidBody->SetRestitution( rest );

		float friction;
		archive.Read( &friction );
		mRigidBody->SetFriction( friction );

		Vector3f angularFactor;
		archive.Read( angularFactor.Ptr(), ar_sizeof( angularFactor ));
		mRigidBody->SetAngularFactor( angularFactor );
		
		Vector3f gravity;
		archive.Read( gravity.Ptr(), ar_sizeof( gravity ));
		mRigidBody->SetGravity( gravity );
	}
}
