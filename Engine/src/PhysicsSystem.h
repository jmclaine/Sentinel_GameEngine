#pragma once
/*
There are two types of PhysicsSystems.

BT uses Bullet physics.

SE stores information for the editor.

Each should save and load identically to
ensure a smooth transition between them.
*/
#include <memory>

#include "Sentinel.h"
#include "Archive.h"
#include "PhysicsShape.h"

namespace Sentinel
{
	class Mesh;
	class Quaternion;
	class Vector3;

	enum PhysicsFlag
	{
		PHYSICS_FLAG_NONE,
		DISABLE_GRAVITY,
		ENABLE_GYROSCOPIC_FORCE,
	};

	////////////////////////////////////////////////////////////////////

	class RigidBody
	{
	protected:

		PhysicsShape* mShape;

		RigidBody();

	public:

		virtual ~RigidBody();

		virtual Vector3 GetPosition() = 0;
		virtual void SetPosition(const Vector3& position) = 0;

		virtual Quaternion GetOrientation() = 0;
		virtual void SetOrientation(const Quaternion& orientation) = 0;

		//////////////////////////////////

		virtual float GetMass() = 0;
		virtual void SetMass(float mass) = 0;

		virtual PhysicsShape* GetShape() = 0;
		virtual void SetShape(PhysicsShape* shape) = 0;

		virtual int GetFlags() = 0;
		virtual void SetFlags(int flags) = 0;

		virtual float GetLinearDamping() = 0;
		virtual float GetAngularDamping() = 0;
		virtual void SetDamping(float linear, float angular) = 0;

		virtual float GetRestitution() = 0;
		virtual void SetRestitution(float rest) = 0;

		virtual float GetFriction() = 0;
		virtual void SetFriction(float friction) = 0;

		virtual Vector3 GetAngularFactor() = 0;
		virtual void SetAngularFactor(const Vector3& factor) = 0;

		virtual Vector3 GetGravity() = 0;
		virtual void SetGravity(const Vector3& gravity) = 0;

		//////////////////////////////////

		virtual void ApplyCentralImpulse(const Vector3& impulse) = 0;
	};

	////////////////////////////////////////////////////////////////////

	// Abstract class designed for either BulletPhysics or SentinelEditor.
	//
	// Call BuildPhysicsSystemBT() or BuildPhysicsSystemSE()
	// to create the PhysicsSystem.
	//
	class PhysicsSystem
	{
		friend class SpherePhysicsShape;
		friend class BoxPhysicsShape;
		friend class CylinderPhysicsShape;
		friend class MeshPhysicsShape;

	private:

		DECLARE_SERIAL_MEMBER_FUNCTION(PhysicsSystem, CreateSphere);
		DECLARE_SERIAL_MEMBER_FUNCTION(PhysicsSystem, CreateBox);
		DECLARE_SERIAL_MEMBER_FUNCTION(PhysicsSystem, CreateCylinder);
		DECLARE_SERIAL_MEMBER_FUNCTION(PhysicsSystem, CreateMesh);

	public:

		virtual ~PhysicsSystem() {}

		virtual void Startup() = 0;

		virtual void Update(float DT) = 0;

		virtual void Shutdown() = 0;

		//////////////////////////////////

		// Load must be called on shape to Create.
		//
		virtual Serializable* CreateSphere() = 0;
		virtual Serializable* CreateBox() = 0;
		virtual Serializable* CreateCylinder() = 0;
		virtual Serializable* CreateMesh() = 0;

		// Shapes are created.
		//
		virtual SpherePhysicsShape* CreateSphere(float radius) = 0;
		virtual BoxPhysicsShape* CreateBox(const Vector3& scale) = 0;
		virtual CylinderPhysicsShape* CreateCylinder(const Vector3& scale) = 0;
		virtual MeshPhysicsShape* CreateMesh(Vector3* verts, UINT count, UINT stride, const Vector3& scale) = 0;

		//////////////////////////////////

		virtual RigidBody* CreateRigidBody(PhysicsShape* shape, const Vector3& position, const Quaternion& orientation, float mass) = 0;

		virtual void AddRigidBody(RigidBody* body) = 0;

		virtual void RemoveRigidBody(RigidBody* body) = 0;
	};

	extern SENTINEL_DLL PhysicsSystem* BuildPhysicsSystemBT();
	extern SENTINEL_DLL PhysicsSystem* BuildPhysicsSystemSE();
}
