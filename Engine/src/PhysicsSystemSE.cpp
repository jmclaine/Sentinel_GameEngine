#include "PhysicsSystem.h"
#include "Vector3f.h"
#include "Quatf.h"

namespace Sentinel
{
	class RigidBodySE : public RigidBody
	{
		friend class PhysicsSystemBT;

	private:

		Vector3f			mPosition;
		Quatf				mOrientation;
		Vector3f			mScale;

		float				mMass;
		PhysicsShapeType	mShapeType;
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
			mLinearDamping	= 0.0f;
			mAngularDamping = 0.0f;
			mRestitution	= 0.0f;
			mFriction		= 0.5f;
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

		PhysicsShapeType GetShapeType()
		{
			return mShapeType;
		}

		void SetShapeType( PhysicsShapeType type )
		{
			mShapeType = type;
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

		RigidBody* CreateSphere( const Vector3f& position, const Quatf& orientation, float radius, float mass )
		{
			RigidBodySE* body = new RigidBodySE();

			body->SetShapeType( PHYSICS_SPHERE );
			body->SetPosition( position );
			body->SetOrientation( orientation );
			body->SetScale( Vector3f( radius, radius, radius ));
			body->SetMass( mass );

			return body;
		}

		RigidBody* CreateBox( const Vector3f& position, const Quatf& orientation, const Vector3f& scale, float mass )
		{
			RigidBodySE* body = new RigidBodySE();

			body->SetShapeType( PHYSICS_BOX );
			body->SetPosition( position );
			body->SetOrientation( orientation );
			body->SetScale( scale );
			body->SetMass( mass );

			return body;
		}

		RigidBody* CreateCylinder( const Vector3f& position, const Quatf& orientation, const Vector3f& scale, float mass )
		{
			RigidBodySE* body = new RigidBodySE();

			body->SetShapeType( PHYSICS_CYLINDER );
			body->SetPosition( position );
			body->SetOrientation( orientation );
			body->SetScale( scale );
			body->SetMass( mass );

			return body;
		}

		RigidBody* CreateMesh( const Vector3f& position, const Quatf& orientation, const Vector3f& scale, Mesh* mesh, float mass )
		{
			RigidBodySE* body = new RigidBodySE();

			body->SetShapeType( PHYSICS_MESH );
			body->SetPosition( position );
			body->SetOrientation( orientation );
			body->SetScale( scale );
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
