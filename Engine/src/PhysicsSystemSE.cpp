#include <vector>

#include "PhysicsSystem.h"
#include "Vector3f.h"
#include "Quatf.h"
#include "Util.h"

namespace Sentinel
{
	class SpherePhysicsShapeSE : public SpherePhysicsShape
	{
	private:

		float mRadius;

	public:

		SpherePhysicsShapeSE() :
			mRadius( 0 )
		{}

		SpherePhysicsShapeSE( float radius )
		{
			Create( radius );
		}

		~SpherePhysicsShapeSE()
		{}

	private:

		void Create( float radius )
		{
			mRadius = radius;
		}

	public:

		float GetRadius()
		{
			return mRadius;
		}

		void SetRadius( float radius )
		{
			mRadius = radius;
		}
	};

	///////////////////////////////////////////////

	class BoxPhysicsShapeSE : public BoxPhysicsShape
	{
	private:

		Vector3f mScale;

	public:

		BoxPhysicsShapeSE()
		{}

		BoxPhysicsShapeSE( const Vector3f& scale )
		{
			Create( scale );
		}

		~BoxPhysicsShapeSE()
		{}

	private:

		void Create( const Vector3f& scale )
		{
			mScale = scale;
		}

	public:

		Vector3f GetScale()
		{
			return mScale;
		}

		void SetScale( const Vector3f& scale )
		{
			mScale = scale;
		}
	};

	///////////////////////////////////////////////

	class CylinderPhysicsShapeSE : public CylinderPhysicsShape
	{
	private:

		Vector3f mScale;

	public:

		CylinderPhysicsShapeSE()
		{}

		CylinderPhysicsShapeSE( const Vector3f& scale )
		{
			Create( scale );
		}

		~CylinderPhysicsShapeSE()
		{}

	private:

		void Create( const Vector3f& scale )
		{
			mScale = scale;
		}

	public:

		Vector3f GetScale()
		{
			return mScale;
		}

		void SetScale( const Vector3f& scale )
		{
			mScale = scale;
		}
	};

	///////////////////////////////////////////////

	class MeshPhysicsShapeSE : public MeshPhysicsShape
	{
	private:

		std::vector< Vector3f > mVert;
		Vector3f				mScale;

	public:

		MeshPhysicsShapeSE()
		{}

		MeshPhysicsShapeSE( Vector3f* verts, UINT count, const Vector3f& scale )
		{
			Create( verts, count, scale );
		}

		~MeshPhysicsShapeSE()
		{}

	private:

		void Create( Vector3f* verts, UINT count, const Vector3f& scale )
		{
			for( UINT x = 0; x < count; ++x )
			{
				AddPoint( verts[ x ] );
			}

			SetScale( scale );
		}

	public:

		int GetNumPoints()
		{
			return (int)mVert.size();
		}

		void AddPoint( const Vector3f& point )
		{
			mVert.push_back( point );
		}

		Vector3f* GetPoints()
		{
			return mVert.data();
		}

		Vector3f GetScale()
		{
			return mScale;
		}

		void SetScale( const Vector3f& scale )
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

		Vector3f			mPosition;
		Quatf				mOrientation;
		Vector3f			mScale;

		float				mMass;
		int					mFlags;

		float				mLinearDamping;
		float				mAngularDamping;
		float				mRestitution;

		float				mFriction;
		Vector3f			mAngularFactor;
		Vector3f			mGravity;

	public:

		RigidBodySE()
		{
			mMass			= 1.0f;
			mFlags			= 0;
			mLinearDamping	= 0.2f;
			mAngularDamping = 0.2f;
			mRestitution	= 0.8f;
			mFriction		= 0.8f;
			mAngularFactor	= Vector3f( 0.8f, 0.8f, 0.8f );
		}

		~RigidBodySE()
		{}

		Vector3f GetPosition()
		{
			return mPosition;
		}

		void SetPosition( const Vector3f& position )
		{
			mPosition = position;
		}

		Quatf GetOrientation()
		{
			return mOrientation;
		}

		void SetOrientation( const Quatf& orientation )
		{
			mOrientation = orientation;
		}

		Vector3f GetScale()
		{
			return mScale;
		}

		void SetScale( const Vector3f& scale )
		{
			mScale = scale;
		}

		//////////////////////////////////

		float GetMass()
		{
			return mMass;
		}

		void SetMass( float mass )
		{
			mMass = mass;
		}

		PhysicsShape* GetShape()
		{
			return mShape;
		}

		void SetShape( PhysicsShape* shape )
		{
			mShape = shape;
		}

		int GetFlags()
		{
			return mFlags;
		}

		void SetFlags( int flags )
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

		void SetDamping( float linear, float angular )
		{
			mLinearDamping  = linear;
			mAngularDamping = angular;
		}
		
		float GetRestitution()
		{
			return mRestitution;
		}

		void SetRestitution( float rest )
		{
			mRestitution = rest;
		}

		float GetFriction()
		{
			return mFriction;
		}

		void SetFriction( float friction )
		{
			mFriction = friction;
		}

		Vector3f GetAngularFactor()
		{
			return mAngularFactor;
		}

		void SetAngularFactor( const Vector3f& factor )
		{
			mAngularFactor = factor;
		}

		Vector3f GetGravity()
		{
			return mGravity;
		}

		void SetGravity( const Vector3f& gravity )
		{
			mGravity = gravity;
		}

		//////////////////////////////////

		void ApplyCentralImpulse( const Vector3f& impulse )
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

		void Update( float DT )
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

		SpherePhysicsShape* CreateSphere( float radius )
		{
			return new SpherePhysicsShapeSE( radius );
		}

		BoxPhysicsShape* CreateBox( const Vector3f& scale )
		{
			return new BoxPhysicsShapeSE( scale );
		}

		CylinderPhysicsShape* CreateCylinder( const Vector3f& scale )
		{
			return new CylinderPhysicsShapeSE( scale );
		}

		MeshPhysicsShape* CreateMesh( Vector3f* verts, UINT count, const Vector3f& scale )
		{
			return new MeshPhysicsShapeSE( verts, count, scale );
		}

		//////////////////////////////////

		RigidBody* CreateRigidBody( PhysicsShape* shape, const Vector3f& position, const Quatf& orientation, float mass )
		{
			RigidBodySE* body = new RigidBodySE();

			body->SetShape( shape );
			body->SetPosition( position );
			body->SetOrientation( orientation );
			body->SetMass( mass );

			return body;
		}

		void AddRigidBody( RigidBody* body )
		{
			// unsupported
		}

		void RemoveRigidBody( RigidBody* body )
		{
			// unsupported
		}
	};

	PhysicsSystem* BuildPhysicsSystemSE()
	{
		return new PhysicsSystemSE();
	}
}
