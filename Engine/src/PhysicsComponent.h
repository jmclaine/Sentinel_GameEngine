#pragma once
/*
Bullet Physics is not compatible with the
managed / wrapped version due to byte-alignment.

All important functionality is inline. The
serial registry creates the PhysicsComponent
reference, and recreates the component using
Clone(). For this reason, creating a new component
to handle special cases would have prevented
the component from being loaded correctly during
execution time.

The #define IGNORE_PHYSICS prevents BulletPhysics
from being initialized, and only stores the values
within the btRigidBody.
*/
#ifndef IGNORE_PHYSICS
#include "PhysicsSystem.h"
#endif

#include "TransformComponent.h"
#include "GameObject.h"

namespace Sentinel
{
	class PhysicsComponent : public GameComponent
	{
		static SerialRegister	mSerialRegistry;
		static Serializeable*	Clone();

	public:

		enum ShapeType
		{
			SHAPE_INVALID,
			SHAPE_SPHERE,
			SHAPE_BOX,
			SHAPE_CYLINDER,
			SHAPE_MESH,
		};

		enum Flag
		{
			DISABLE_GRAVITY			= 1,
			ENABLE_GYROSCOPIC_FORCE = 2,
		};

	private:

#ifndef IGNORE_PHYSICS
	PhysicsComponent::ShapeType PhysicsComponent::ToSentinelShapeType( int type )
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

	protected:

		TransformComponent*		mTransform;

#ifndef IGNORE_PHYSICS
		btRigidBody*			mRigidBody;
#else
		public:

		// These values are required in order to
		// save and load the physics back into
		// BulletPhysics.
		//
		ShapeType			mShapeType;
		float				mMass;

		Vector3f			mShapePosition;
		Quatf				mShapeOrientation;
		Vector3f			mShapeScale;

		int					mFlags;
		float				mLinearDamping;
		float				mAngularDamping;
		float				mRestitution;
		float				mFriction;

		Vector3f			mAngularFactor;
		Vector3f			mGravity;
#endif

	public:

		PhysicsComponent()
		{
			mType = GameComponent::PHYSICS;

			mTransform		= NULL;

#ifdef IGNORE_PHYSICS
			mShapeType		= SHAPE_INVALID;
			mMass			= 1.0f;

			mShapeScale		= Vector3f( 1, 1, 1 );

			mFlags			= 0;
			mLinearDamping	= 0.0f;
			mAngularDamping	= 0.0f;
			mRestitution	= 0.0f;
			mFriction		= 0.5f;
#endif
		}

#ifndef IGNORE_PHYSICS
		PhysicsComponent( btRigidBody* body )
		{
			mType = GameComponent::PHYSICS;

			mTransform = NULL;

			mRigidBody = body;
		}
#endif
		~PhysicsComponent()
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

		void Startup()
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

		void Update()
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

		void Shutdown()
		{
			mTransform = NULL;

#ifndef IGNORE_PHYSICS
			PhysicsSystem::Inst()->RemoveRigidBody( mRigidBody );
#endif
		}

		/////////////////////////////////

#ifndef IGNORE_PHYSICS
		btRigidBody*	GetRigidBody();
		void			SetRigidBody( btRigidBody* body );
#endif

		void Save( Archive& archive )
		{
			mSerialRegistry.Save( archive );

			GameComponent::Save( archive );

#ifndef IGNORE_PHYSICS
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

			//if( type == SHAPE_MESH )

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
#else
			int type = (int)mShapeType;
		
			if( type == SHAPE_INVALID )
				REPORT_ERROR( "No support for EditorPhysicsComponent shape on " << mOwner->mName, "Save Error" );

			archive.Write( &type );

			archive.Write( &mMass );

			archive.Write( mShapePosition.Ptr(),	ar_sizeof( mShapePosition ));
			archive.Write( mShapeOrientation.Ptr(), ar_sizeof( mShapeOrientation ));
			archive.Write( mShapeScale.Ptr(),		ar_sizeof( mShapeScale ));

			archive.Write( &mFlags );
			archive.Write( &mLinearDamping );
			archive.Write( &mAngularDamping );
			archive.Write( &mRestitution );
			archive.Write( &mFriction );

			archive.Write( mAngularFactor.Ptr(),	ar_sizeof( mAngularFactor ));
			archive.Write( mGravity.Ptr(),			ar_sizeof( mGravity ));
#endif
		}

		void Load( Archive& archive )
		{
			GameComponent::Load( archive );

#ifndef IGNORE_PHYSICS
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
					/*{
						std::string name;
						archive.Read( &name );

						std::shared_ptr< Mesh > mesh = MeshManager::Inst()->Get( name );

						if( mesh )
							mRigidBody = PhysicsSystem::Inst()->CreateMesh( pos, rot, scale, mesh.get(), mass );
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
#else
			int type;
			archive.Read( &type );

			if( type == SHAPE_INVALID )
				REPORT_ERROR( "No support for EditorPhysicsComponent shape on " << mOwner->mName, "Save Error" );

			mShapeType = (ShapeType)type;

			archive.Read( &mMass );

			archive.Read( mShapePosition.Ptr(),		ar_sizeof( mShapePosition ));
			archive.Read( mShapeOrientation.Ptr(),	ar_sizeof( mShapeOrientation ));
			archive.Read( mShapeScale.Ptr(),		ar_sizeof( mShapeScale ));

			archive.Read( &mFlags );
			archive.Read( &mLinearDamping );
			archive.Read( &mAngularDamping );
			archive.Read( &mRestitution );
			archive.Read( &mFriction );

			archive.Read( mAngularFactor.Ptr(),		ar_sizeof( mAngularFactor ));
			archive.Read( mGravity.Ptr(),			ar_sizeof( mGravity ));
#endif
		}
	};
}
