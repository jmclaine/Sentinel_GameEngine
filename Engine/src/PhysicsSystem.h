#pragma once
/*
There are two types of PhysicsSystems.

BT uses Bullet physics.

SE stores information for the editor.

Each should save and load identically to
ensure a smooth transition between them.
*/
#include <memory>

#include "Common.h"

namespace Sentinel
{
	class Mesh;
	class Quatf;
	class Vector3f;

	enum PhysicsShapeType
	{
		PHYSICS_INVALID,
		PHYSICS_SPHERE,
		PHYSICS_BOX,
		PHYSICS_CYLINDER,
		PHYSICS_MESH,
	};

	enum PhysicsFlag
	{
		PHYSICS_FLAG_NONE,
		DISABLE_GRAVITY,
		ENABLE_GYROSCOPIC_FORCE,
	};

	class RigidBody
	{
	public:

		std::shared_ptr< Mesh > mMesh;

		virtual ~RigidBody() {}

		virtual Vector3f	GetPosition() = 0;
		virtual void		SetPosition( const Vector3f& position ) = 0;

		virtual Quatf		GetOrientation() = 0;
		virtual void		SetOrientation( const Quatf& orientation ) = 0;

		virtual Vector3f	GetScale() = 0;
		virtual void		SetScale( const Vector3f& scale ) = 0;

		//////////////////////////////////

		virtual float		GetMass() = 0;
		virtual void		SetMass( float mass ) = 0;

		virtual PhysicsShapeType GetShapeType() = 0;
		virtual void		SetShapeType( PhysicsShapeType type ) = 0;
		
		virtual int			GetFlags() = 0;
		virtual void		SetFlags( int flags ) = 0;

		virtual float		GetLinearDamping() = 0;
		virtual float		GetAngularDamping() = 0;
		virtual void		SetDamping( float linear, float angular ) = 0;

		virtual float		GetRestitution() = 0;
		virtual void		SetRestitution( float rest ) = 0;

		virtual float		GetFriction() = 0;
		virtual void		SetFriction( float friction ) = 0;

		virtual Vector3f	GetAngularFactor() = 0;
		virtual void		SetAngularFactor( const Vector3f& factor ) = 0;

		virtual Vector3f	GetGravity() = 0;
		virtual void		SetGravity( const Vector3f& gravity ) = 0;

		//////////////////////////////////

		virtual void		ApplyCentralImpulse( const Vector3f& impulse ) = 0;
	};

	////////////////////////////////////////////////////////////////////

	// Abstract class designed for either BulletPhysics or SentinelEditor.
	//
	// Call BuildPhysicsSystemBT() or BuildPhysicsSystemSE()
	// to create the PhysicsSystem.
	//
	class PhysicsSystem
	{
	public:

		virtual ~PhysicsSystem() {}

		virtual void			Startup() = 0;

		virtual void			Update( float DT ) = 0;

		virtual void			Shutdown() = 0;

		//////////////////////////////////

		virtual RigidBody*		CreateSphere( const Vector3f& position, const Quatf& orientation, float radius, float mass ) = 0;

		virtual RigidBody*		CreateBox( const Vector3f& position, const Quatf& orientation, const Vector3f& scale, float mass ) = 0;

		virtual RigidBody*		CreateCylinder( const Vector3f& position, const Quatf& orientation, const Vector3f& scale, float mass ) = 0;

		virtual RigidBody*		CreateMesh( const Vector3f& position, const Quatf& orientation, const Vector3f& scale, std::shared_ptr< Mesh > mesh, float mass ) = 0;

		//////////////////////////////////

		virtual void			AddRigidBody( RigidBody* body ) = 0;

		virtual void			RemoveRigidBody( RigidBody* body ) = 0;
	};

	extern SENTINEL_DLL PhysicsSystem* BuildPhysicsSystemBT();
	extern SENTINEL_DLL PhysicsSystem* BuildPhysicsSystemSE();
}
