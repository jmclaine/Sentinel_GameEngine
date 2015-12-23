/*
PhysicsSystem based on BulletPhysics.
*/
#include <process.h>

#include "btBulletDynamicsCommon.h"

#ifdef NDEBUG
#pragma comment (lib, "Release/BulletDynamics.lib")
#pragma comment (lib, "Release/BulletCollision.lib")
#pragma comment (lib, "Release/LinearMath.lib")
#else
#pragma comment (lib, "Debug/BulletDynamics_Debug.lib")
#pragma comment (lib, "Debug/BulletCollision_Debug.lib")
#pragma comment (lib, "Debug/LinearMath_Debug.lib")
#endif

#include "PhysicsSystem.h"
#include "Vector3.h"
#include "Quaternion.h"

namespace Sentinel
{
	class PhysicsSystemBT;

	///////////////////////////////////////////////

	class SpherePhysicsShapeBT : public SpherePhysicsShape
	{
	private:

		btSphereShape* mShape;

	public:

		SpherePhysicsShapeBT()
		{
			mShape = new btSphereShape(1);
		}

		SpherePhysicsShapeBT(float radius)
		{
			mShape = new btSphereShape(1);

			Create(radius);
		}

		~SpherePhysicsShapeBT()
		{
			delete mShape;
		}

		operator btSphereShape* ()
		{
			return mShape;
		}

		void* GetData()
		{
			return mShape;
		}

	public:

		float GetRadius()
		{
			return mShape->getRadius();
		}

		void SetRadius(float radius)
		{
			mShape->setUnscaledRadius(radius);
		}
	};

	///////////////////////////////////////////////

	class BoxPhysicsShapeBT : public BoxPhysicsShape
	{
	private:

		btBoxShape* mShape;

	public:

		BoxPhysicsShapeBT()
		{
			mShape = new btBoxShape(btVector3(1, 1, 1));
		}

		BoxPhysicsShapeBT(const Vector3& scale)
		{
			mShape = new btBoxShape(btVector3(1, 1, 1));

			Create(scale);
		}

		~BoxPhysicsShapeBT()
		{
			delete mShape;
		}

		operator btBoxShape* ()
		{
			return mShape;
		}

		void* GetData()
		{
			return mShape;
		}

	public:

		Vector3 GetScale()
		{
			const btVector3& v = mShape->getLocalScaling();

			return Vector3(v.x(), v.y(), v.z());
		}

		void SetScale(const Vector3& scale)
		{
			mShape->setLocalScaling(btVector3(scale.x, scale.y, scale.z));
		}
	};

	///////////////////////////////////////////////

	class CylinderPhysicsShapeBT : public CylinderPhysicsShape
	{
	private:

		btCylinderShape* mShape;

	public:

		CylinderPhysicsShapeBT()
		{
			mShape = new btCylinderShape(btVector3(1, 1, 1));
		}

		CylinderPhysicsShapeBT(const Vector3& scale)
		{
			mShape = new btCylinderShape(btVector3(1, 1, 1));

			Create(scale);
		}

		~CylinderPhysicsShapeBT()
		{
			delete mShape;
		}

		operator btCylinderShape* ()
		{
			return mShape;
		}

		void* GetData()
		{
			return mShape;
		}

	public:

		Vector3 GetScale()
		{
			const btVector3& v = mShape->getLocalScaling();

			return Vector3(v.x(), v.y(), v.z());
		}

		void SetScale(const Vector3& scale)
		{
			mShape->setLocalScaling(btVector3(scale.x, scale.y, scale.z));
		}
	};

	///////////////////////////////////////////////

	class MeshPhysicsShapeBT : public MeshPhysicsShape
	{
	private:

		btConvexHullShape* mShape;

	public:

		MeshPhysicsShapeBT()
		{
			mShape = new btConvexHullShape();
		}

		MeshPhysicsShapeBT(void* verts, UINT count, UINT stride, const Vector3& scale)
		{
			mShape = new btConvexHullShape();

			Create(verts, count, stride, scale);
		}

		~MeshPhysicsShapeBT()
		{
			delete mShape;
		}

		operator btConvexHullShape* ()
		{
			return mShape;
		}

		void* GetData()
		{
			return mShape;
		}

	public:

		int GetNumPoints()
		{
			return mShape->getNumPoints();
		}

		void AddPoint(const Vector3& point)
		{
			mShape->addPoint(btVector3(point.x, point.y, point.z));
		}

		Vector3* GetPoints()
		{
			_ASSERT(0);	// unscaled points are 4 floats

			return (Vector3*)mShape->getUnscaledPoints();
		}

		Vector3 GetScale()
		{
			const btVector3& v = mShape->getLocalScaling();

			return Vector3(v.x(), v.y(), v.z());
		}

		void SetScale(const Vector3& scale)
		{
			mShape->setLocalScaling(btVector3(scale.x, scale.y, scale.z));
		}
	};

	///////////////////////////////////////////////

	class CompoundPhysicsShapeBT : public CompoundPhysicsShape
	{
	private:

		btCompoundShape* mShape;

	public:

		CompoundPhysicsShapeBT()
		{
			mShape = new btCompoundShape();
		}

		~CompoundPhysicsShapeBT()
		{
			delete mShape;
		}

		operator btCompoundShape* ()
		{
			return mShape;
		}

		void* GetData()
		{
			return mShape;
		}
	};

	/////////////////////////////////////////////////////////////////////////////

	class RigidBodyBT : public RigidBody
	{
		friend class PhysicsSystemBT;

	private:

		btRigidBody* mRigidBody;

	public:

		RigidBodyBT(PhysicsShape* shape, btRigidBody* body)
		{
			mShape = shape;
			mRigidBody = body;
		}

		~RigidBodyBT()
		{
			delete mRigidBody->getMotionState();
			delete mRigidBody;
		}

		Vector3 GetPosition()
		{
			btVector3 v(mRigidBody->getCenterOfMassPosition());

			return Vector3(v.x(), v.y(), v.z());
		}

		void SetPosition(const Vector3& position)
		{
			mRigidBody->getWorldTransform().setOrigin(btVector3(position.x, position.y, position.z));
		}

		Quaternion GetOrientation()
		{
			btQuaternion q(mRigidBody->getOrientation());

			return Quaternion(q.x(), q.y(), q.z(), q.w());
		}

		void SetOrientation(const Quaternion& orientation)
		{
			mRigidBody->getWorldTransform().setRotation(btQuaternion(orientation.x, orientation.y, orientation.z, orientation.w));
		}

		//////////////////////////////////

		float GetMass()
		{
			return 1.0f / mRigidBody->getInvMass();
		}

		void SetMass(float mass)
		{
			mRigidBody->setMassProps(mass, btVector3());
		}

		PhysicsShape* GetShape()
		{
			return mShape;
		}

		void SetShape(PhysicsShape* shape)
		{
			_ASSERT(0); // unsupported
		}

		int GetFlags()
		{
			return mRigidBody->getFlags();
		}

		void SetFlags(int flags)
		{
			mRigidBody->setFlags(flags);
		}

		float GetLinearDamping()
		{
			return mRigidBody->getLinearDamping();
		}

		float GetAngularDamping()
		{
			return mRigidBody->getAngularDamping();
		}

		void SetDamping(float linear, float angular)
		{
			mRigidBody->setDamping(linear, angular);
		}

		float GetRestitution()
		{
			return mRigidBody->getRestitution();
		}

		void SetRestitution(float rest)
		{
			mRigidBody->setRestitution(rest);
		}

		float GetFriction()
		{
			return mRigidBody->getFriction();
		}

		void SetFriction(float friction)
		{
			mRigidBody->setFriction(friction);
		}

		Vector3 GetAngularFactor()
		{
			btVector3 v(mRigidBody->getAngularFactor());

			return Vector3(v.x(), v.y(), v.z());
		}

		void SetAngularFactor(const Vector3& factor)
		{
			mRigidBody->setAngularFactor(btVector3(factor.x, factor.y, factor.z));
		}

		Vector3 GetGravity()
		{
			btVector3 v(mRigidBody->getGravity());

			return Vector3(v.x(), v.y(), v.z());
		}

		void SetGravity(const Vector3& gravity)
		{
			mRigidBody->setGravity(btVector3(gravity.x, gravity.y, gravity.z));
		}

		//////////////////////////////////

		void ApplyCentralImpulse(const Vector3& impulse)
		{
			mRigidBody->activate(true);
			mRigidBody->applyCentralImpulse(btVector3(impulse.x, impulse.y, impulse.z));
		}
	};

	/////////////////////////////////////////////////////////////////////////////

	class PhysicsSystemBT : public PhysicsSystem
	{
	private:

		btBroadphaseInterface* mBroadphase;
		btDefaultCollisionConfiguration* mConfig;
		btCollisionDispatcher* mDispatcher;
		btSequentialImpulseConstraintSolver* mSolver;
		btDiscreteDynamicsWorld* mWorld;

		bool mIsActive;

	public:

		PhysicsSystemBT()
		{
			mBroadphase = new btDbvtBroadphase();
			mConfig = new btDefaultCollisionConfiguration();
			mDispatcher = new btCollisionDispatcher(mConfig);
			mSolver = new btSequentialImpulseConstraintSolver;

			mWorld = new btDiscreteDynamicsWorld(mDispatcher, mBroadphase, mSolver, mConfig);
			mWorld->setGravity(btVector3(0, -10, 0));

			mIsActive = false;
		}

		~PhysicsSystemBT()
		{
			SAFE_DELETE(mWorld);
			SAFE_DELETE(mDispatcher);
			SAFE_DELETE(mConfig);
			SAFE_DELETE(mBroadphase);
			SAFE_DELETE(mSolver);
		}

	public:

		void Startup()
		{
			mIsActive = true;
		}

		void Update(float DT)
		{
			if (mIsActive)
			{
				mWorld->stepSimulation(btScalar(DT));
			}
		}

		void Shutdown()
		{
			mIsActive = false;
		}

		//////////////////////////////////

		Serializable* CreateSphere()
		{
			return new SpherePhysicsShapeBT();
		}

		Serializable* CreateBox()
		{
			return new BoxPhysicsShapeBT();
		}

		Serializable* CreateCylinder()
		{
			return new CylinderPhysicsShapeBT();
		}

		Serializable* CreateMesh()
		{
			return new MeshPhysicsShapeBT();
		}

		//////////////////////////////////

		SpherePhysicsShape* CreateSphere(float radius)
		{
			return new SpherePhysicsShapeBT(radius);
		}

		BoxPhysicsShape* CreateBox(const Vector3& scale)
		{
			return new BoxPhysicsShapeBT(scale);
		}

		CylinderPhysicsShape* CreateCylinder(const Vector3& scale)
		{
			return new CylinderPhysicsShapeBT(scale);
		}

		MeshPhysicsShape* CreateMesh(Vector3* verts, UINT count, UINT stride, const Vector3& scale)
		{
			return new MeshPhysicsShapeBT(verts, count, stride, scale);
		}

		//////////////////////////////////
		
		RigidBody* CreateRigidBody(PhysicsShape* shape, const Vector3& position, const Quaternion& orientation, float mass)
		{
			btScalar btMass(mass);
			btVector3 inertia(0, 10, 0);

			btCollisionShape* btShape = static_cast<btCollisionShape*>(shape->GetData());

			if (btMass != 0.0f)
				btShape->calculateLocalInertia(btMass, inertia);

			btMotionState* motion = new btDefaultMotionState(btTransform(
				btQuaternion(orientation.x, orientation.y, orientation.z, orientation.w),
				btVector3(position.x, position.y, position.z)));
			
			btRigidBody* body = new btRigidBody(btMass, motion, btShape, inertia);

			return new RigidBodyBT(shape, body);
		}

		void AddRigidBody(RigidBody* body)
		{
			btRigidBody* btBody = static_cast<RigidBodyBT*>(body)->mRigidBody;

			mWorld->addRigidBody(btBody);
		}

		void RemoveRigidBody(RigidBody* body)
		{
			btRigidBody* btBody = static_cast<RigidBodyBT*>(body)->mRigidBody;

			mWorld->removeRigidBody(btBody);
		}
	};

	PhysicsSystem* BuildPhysicsSystemBT()
	{
		return new PhysicsSystemBT();
	}
}
