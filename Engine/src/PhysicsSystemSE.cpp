#include <vector>

#include "PhysicsSystem.h"
#include "Vector3.h"
#include "Quaternion.h"
#include "Util.h"

namespace Sentinel
{
	class SpherePhysicsShapeSE : public SpherePhysicsShape
	{
	private:

		float mRadius;

	public:

		SpherePhysicsShapeSE() :
			mRadius(0)
		{}

		SpherePhysicsShapeSE(float radius)
		{
			Create(radius);
		}

		~SpherePhysicsShapeSE()
		{}

	public:

		float GetRadius()
		{
			return mRadius;
		}

		void SetRadius(float radius)
		{
			mRadius = radius;
		}
	};

	///////////////////////////////////////////////

	class BoxPhysicsShapeSE : public BoxPhysicsShape
	{
	private:

		Vector3 mScale;

	public:

		BoxPhysicsShapeSE()
		{}

		BoxPhysicsShapeSE(const Vector3& scale)
		{
			Create(scale);
		}

		~BoxPhysicsShapeSE()
		{}

	public:

		Vector3 GetScale()
		{
			return mScale;
		}

		void SetScale(const Vector3& scale)
		{
			mScale = scale;
		}
	};

	///////////////////////////////////////////////

	class CylinderPhysicsShapeSE : public CylinderPhysicsShape
	{
	private:

		Vector3 mScale;

	public:

		CylinderPhysicsShapeSE()
		{}

		CylinderPhysicsShapeSE(const Vector3& scale)
		{
			Create(scale);
		}

		~CylinderPhysicsShapeSE()
		{}

	public:

		Vector3 GetScale()
		{
			return mScale;
		}

		void SetScale(const Vector3& scale)
		{
			mScale = scale;
		}
	};

	///////////////////////////////////////////////

	class MeshPhysicsShapeSE : public MeshPhysicsShape
	{
	private:

		std::vector<Vector3> mVert;
		Vector3 mScale;

	public:

		MeshPhysicsShapeSE()
		{}

		MeshPhysicsShapeSE(void* verts, UINT count, UINT stride, const Vector3& scale)
		{
			Create(verts, count, stride, scale);
		}

		~MeshPhysicsShapeSE()
		{}

	public:

		int GetNumPoints()
		{
			return (int)mVert.size();
		}

		void AddPoint(const Vector3& point)
		{
			mVert.push_back(point);
		}

		Vector3* GetPoints()
		{
			return mVert.data();
		}

		Vector3 GetScale()
		{
			return mScale;
		}

		void SetScale(const Vector3& scale)
		{
			mScale = scale;
		}
	};

	///////////////////////////////////////////////

	class CompoundPhysicsShapeSE : public CompoundPhysicsShape
	{
	public:

		CompoundPhysicsShapeSE()
		{}

		~CompoundPhysicsShapeSE()
		{}
	};

	/////////////////////////////////////////////////////////////////////////////

	class RigidBodySE : public RigidBody
	{
	private:

		Vector3 mPosition;
		Quaternion mOrientation;

		float mMass;
		int mFlags;

		float mLinearDamping;
		float mAngularDamping;
		float mRestitution;

		float mFriction;
		Vector3 mAngularFactor;
		Vector3 mGravity;

	public:

		RigidBodySE()
		{
			mMass = 1.0f;
			mFlags = 0;
			mLinearDamping = 0.2f;
			mAngularDamping = 0.2f;
			mRestitution = 0.8f;
			mFriction = 0.8f;
			mAngularFactor = Vector3(0.8f, 0.8f, 0.8f);
		}

		~RigidBodySE()
		{}

		Vector3 GetPosition()
		{
			return mPosition;
		}

		void SetPosition(const Vector3& position)
		{
			mPosition = position;
		}

		Quaternion GetOrientation()
		{
			return mOrientation;
		}

		void SetOrientation(const Quaternion& orientation)
		{
			mOrientation = orientation;
		}

		//////////////////////////////////

		float GetMass()
		{
			return mMass;
		}

		void SetMass(float mass)
		{
			mMass = mass;
		}

		PhysicsShape* GetShape()
		{
			return mShape;
		}

		void SetShape(PhysicsShape* shape)
		{
			mShape = shape;
		}

		int GetFlags()
		{
			return mFlags;
		}

		void SetFlags(int flags)
		{
			mFlags = flags;
		}

		float GetLinearDamping()
		{
			return mLinearDamping;
		}

		float GetAngularDamping()
		{
			return mAngularDamping;
		}

		void SetDamping(float linear, float angular)
		{
			mLinearDamping = linear;
			mAngularDamping = angular;
		}

		float GetRestitution()
		{
			return mRestitution;
		}

		void SetRestitution(float rest)
		{
			mRestitution = rest;
		}

		float GetFriction()
		{
			return mFriction;
		}

		void SetFriction(float friction)
		{
			mFriction = friction;
		}

		Vector3 GetAngularFactor()
		{
			return mAngularFactor;
		}

		void SetAngularFactor(const Vector3& factor)
		{
			mAngularFactor = factor;
		}

		Vector3 GetGravity()
		{
			return mGravity;
		}

		void SetGravity(const Vector3& gravity)
		{
			mGravity = gravity;
		}

		//////////////////////////////////

		void ApplyCentralImpulse(const Vector3& impulse)
		{
			_ASSERT(0); // unsupported
		}
	};

	class PhysicsSystemSE : public PhysicsSystem
	{
	public:

		PhysicsSystemSE()
		{}

		~PhysicsSystemSE()
		{}

	public:

		void Startup()
		{
			// unsupported
		}

		void Update(float DT)
		{
			// unsupported
		}

		void Shutdown()
		{
			// unsupported
		}

		//////////////////////////////////

		Serializable* CreateSphere()
		{
			return new SpherePhysicsShapeSE();
		}

		Serializable* CreateBox()
		{
			return new BoxPhysicsShapeSE();
		}

		Serializable* CreateCylinder()
		{
			return new CylinderPhysicsShapeSE();
		}

		Serializable* CreateMesh()
		{
			return new MeshPhysicsShapeSE();
		}

		//////////////////////////////////

		SpherePhysicsShape* CreateSphere(float radius)
		{
			return new SpherePhysicsShapeSE(radius);
		}

		BoxPhysicsShape* CreateBox(const Vector3& scale)
		{
			return new BoxPhysicsShapeSE(scale);
		}

		CylinderPhysicsShape* CreateCylinder(const Vector3& scale)
		{
			return new CylinderPhysicsShapeSE(scale);
		}

		MeshPhysicsShape* CreateMesh(Vector3* verts, UINT count, UINT stride, const Vector3& scale)
		{
			return new MeshPhysicsShapeSE(verts, count, stride, scale);
		}

		//////////////////////////////////

		RigidBody* CreateRigidBody(PhysicsShape* shape, const Vector3& position, const Quaternion& orientation, float mass)
		{
			RigidBodySE* body = new RigidBodySE();

			body->SetShape(shape);
			body->SetPosition(position);
			body->SetOrientation(orientation);
			body->SetMass(mass);

			return body;
		}

		void AddRigidBody(RigidBody* body)
		{
			// unsupported
		}

		void RemoveRigidBody(RigidBody* body)
		{
			// unsupported
		}
	};

	PhysicsSystem* BuildPhysicsSystemSE()
	{
		return new PhysicsSystemSE();
	}
}
