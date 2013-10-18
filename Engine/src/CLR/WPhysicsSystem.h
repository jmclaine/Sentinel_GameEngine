#pragma once
/*
There are two types of PhysicsSystems.

BT uses Bullet physics.

SE stores information for the editor.

Each should save and load identically to
ensure a smooth transition between them.
*/
#include "Property.h"
#include "PhysicsSystem.h"

namespace Sentinel { namespace Wrapped
{
	ref class WMesh;
	ref class WQuatf;
	ref class WVector3f;

	public enum class PhysicsShapeType
	{
		INVALID,
		SPHERE,
		BOX,
		CYLINDER,
		MESH,
	};

	public enum class PhysicsFlag
	{
		NONE,
		DISABLE_GRAVITY,
		ENABLE_GYROSCOPIC_FORCE,
	};

	public ref class WRigidBody
	{
		DECLARE_REF_PTR( RigidBody );

	public:

		WRigidBody( RigidBody* body );

		void			Release();

		DECLARE_OP_PTR( RigidBody );

		DECLARE_PROPERTY( WVector3f^,		Position );
		DECLARE_PROPERTY( WQuatf^,			Orientation );
		DECLARE_PROPERTY( WVector3f^,		Scale );
		
		//////////////////////////////////

		DECLARE_PROPERTY( float,			Mass );
		DECLARE_PROPERTY( PhysicsShapeType,	ShapeType );
		DECLARE_PROPERTY( int,				Flags );
		
		DECLARE_PROPERTY_G( float,			LinearDamping );
		DECLARE_PROPERTY_G( float,			AngularDamping );

		void			SetDamping( float linear, float angular );

		DECLARE_PROPERTY( float,			Restitution );
		DECLARE_PROPERTY( float,			Friction );
		
		DECLARE_PROPERTY( WVector3f^,		AngularFactor );
		DECLARE_PROPERTY( WVector3f^,		Gravity );
		
		//////////////////////////////////

		void			ApplyCentralImpulse( WVector3f^ impulse );
	};

	////////////////////////////////////////////////////////////////////

	public ref class WPhysicsSystem
	{
		DECLARE_REF_PTR( PhysicsSystem );

	public:

		WPhysicsSystem( PhysicsSystem* physics );

		void			Release();

		DECLARE_OP_PTR( PhysicsSystem );

		void			Startup();

		void			Update( float DT );

		void			Shutdown();

		//////////////////////////////////

		WRigidBody^		CreateSphere( WVector3f^ position, WQuatf^ orientation, float radius, float mass );

		WRigidBody^		CreateBox( WVector3f^ position, WQuatf^ orientation, WVector3f^ scale, float mass );

		WRigidBody^		CreateCylinder( WVector3f^ position, WQuatf^ orientation, WVector3f^ scale, float mass );

		WRigidBody^		CreateMesh( WVector3f^ position, WQuatf^ orientation, WVector3f^ scale, WMesh^ mesh, float mass );

		//////////////////////////////////

		void			AddRigidBody( WRigidBody^ body );

		void			RemoveRigidBody( WRigidBody^ body );

		//////////////////////////////////

		static WPhysicsSystem^ BuildBT();
		static WPhysicsSystem^ BuildSE();
	};
}}
