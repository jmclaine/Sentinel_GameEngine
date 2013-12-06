/*
PhysicsSystem based on BulletPhysics.
*/
#include <process.h>

#include "btBulletDynamicsCommon.h"

#ifdef NDEBUG
	#pragma comment (lib, "BulletCollision_vs2010.lib")
	#pragma comment (lib, "BulletDynamics_vs2010.lib")
	#pragma comment (lib, "LinearMath_vs2010.lib")
#else
	#pragma comment (lib, "BulletCollision_vs2010_debug.lib")
	#pragma comment (lib, "BulletDynamics_vs2010_debug.lib")
	#pragma comment (lib, "LinearMath_vs2010_debug.lib")
#endif

#include "PhysicsSystem.h"
#include "Mesh.h"
#include "Timing.h"
#include "Util.h"
#include "Buffer.h"
#include "Vector3f.h"
#include "Quatf.h"

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
			Create( 1 );
		}

		SpherePhysicsShapeBT( float radius )
		{
			Create( radius );
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

	private:

		void Create( float radius )
		{
			mShape = new btSphereShape( btScalar( radius ));
		}

	public:

		float GetRadius()
		{
			return mShape->getRadius();
		}

		void SetRadius( float radius )
		{
			mShape->setUnscaledRadius( radius );
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
			Create( Vector3f( 1, 1, 1 ));
		}

		BoxPhysicsShapeBT( const Vector3f& scale )
		{
			Create( scale );
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

	private:

		void Create( const Vector3f& scale )
		{
			mShape = new btBoxShape( btVector3( 1, 1, 1 ));

			SetScale( scale );
		}

	public:

		Vector3f GetScale()
		{
			const btVector3& v = mShape->getLocalScaling();

			return Vector3f( v.x(), v.y(), v.z() );
		}

		void SetScale( const Vector3f& scale )
		{
			mShape->setLocalScaling( btVector3( scale.x, scale.y, scale.z ));
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
			Create( Vector3f( 1, 1, 1 ));
		}

		CylinderPhysicsShapeBT( const Vector3f& scale )
		{
			Create( scale );
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

	private:

		void Create( const Vector3f& scale )
		{
			mShape = new btCylinderShape( btVector3( 1, 0.5f, 1 ));

			SetScale( scale );
		}

	public:

		Vector3f GetScale()
		{
			const btVector3& v = mShape->getLocalScaling();

			return Vector3f( v.x(), v.y(), v.z() );
		}

		void SetScale( const Vector3f& scale )
		{
			mShape->setLocalScaling( btVector3( scale.x, scale.y, scale.z ));
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

		MeshPhysicsShapeBT( Vector3f* verts, UINT count, const Vector3f& scale )
		{
			Create( verts, count, scale );
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

	private:

		void Create( Vector3f* verts, UINT count, const Vector3f& scale )
		{
			/*
			mMesh = mesh;

			Buffer* vbo = mesh->mVBO;
			
			UINT   count   = vbo->Count();
			UCHAR* vboData = (UCHAR*)vbo->Lock();
			
			btConvexHullShape* shape = new btConvexHullShape();
			Vector3f v;

			for( UINT x = 0; x < count; ++x )
			{
				v = *(Vector3f*)vboData;
				shape->addPoint( btVector3( v.x, v.y, v.z ));
				vboData += vbo->Stride();
			}
			vbo->Unlock();
			*/

			mShape = new btConvexHullShape();

			for( UINT x = 0; x < count; ++x )
			{
				const Vector3f& v( verts[ x ] );

				mShape->addPoint( btVector3( v.x, v.y, v.z ));
			}

			mShape->setLocalScaling( btVector3( scale.x, scale.y, scale.z ));
		}

	public:

		int GetNumPoints()
		{
			return mShape->getNumPoints();
		}

		void AddPoint( const Vector3f& point )
		{
			mShape->addPoint( btVector3( point.x, point.y, point.z ));
		}

		Vector3f* GetPoints()
		{
			_ASSERT( 0 );	// ** unscaled points are 4 floats **

			return (Vector3f*)mShape->getUnscaledPoints();
		}

		Vector3f GetScale()
		{
			const btVector3& v = mShape->getLocalScaling();

			return Vector3f( v.x(), v.y(), v.z() );
		}

		void SetScale( const Vector3f& scale )
		{
			mShape->setLocalScaling( btVector3( scale.x, scale.y, scale.z ));
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

		btRigidBody*	mRigidBody;

	public:

		RigidBodyBT( PhysicsShape* shape, btRigidBody* body )
		{
			mShape     = shape;
			mRigidBody = body;
		}

		~RigidBodyBT()
		{
			delete mRigidBody->getMotionState();
			delete mRigidBody;
		}

		Vector3f GetPosition()
		{
			btVector3 v( mRigidBody->getCenterOfMassPosition() );

			return Vector3f( v.x(), v.y(), v.z() );
		}

		void SetPosition( const Vector3f& position )
		{
			mRigidBody->getWorldTransform().setOrigin( btVector3( position.x, position.y, position.z ));
		}

		Quatf GetOrientation()
		{
			btQuaternion q( mRigidBody->getOrientation() );

			return Quatf( q.x(), q.y(), q.z(), q.w() );
		}

		void SetOrientation( const Quatf& orientation )
		{
			mRigidBody->getWorldTransform().setRotation( btQuaternion( orientation.x, orientation.y, orientation.z, orientation.w ));
		}

		Vector3f GetScale()
		{
			btVector3 v( mRigidBody->getCollisionShape()->getLocalScaling() );

			return Vector3f( v.x(), v.y(), v.z() );
		}

		void SetScale( const Vector3f& scale )
		{
			mRigidBody->getCollisionShape()->setLocalScaling( btVector3( scale.x, scale.y, scale.z ));
		}

		//////////////////////////////////

		float GetMass()
		{
			return 1.0f / mRigidBody->getInvMass();
		}

		void SetMass( float mass )
		{
			mRigidBody->setMassProps( mass, btVector3() );
		}

		PhysicsShape* GetShape()
		{
			return mShape;
		}

		void SetShape( PhysicsShape* shape )
		{
			_ASSERT(0); // unsupported
		}

		int GetFlags()
		{
			return mRigidBody->getFlags();
		}

		void SetFlags( int flags )
		{
			mRigidBody->setFlags( flags );
		}

		float GetLinearDamping()
		{
			return mRigidBody->getLinearDamping();
		}

		float GetAngularDamping()
		{
			return mRigidBody->getAngularDamping();
		}

		void SetDamping( float linear, float angular )
		{
			mRigidBody->setDamping( linear, angular );
		}
		
		float GetRestitution()
		{
			return mRigidBody->getRestitution();
		}

		void SetRestitution( float rest )
		{
			mRigidBody->setRestitution( rest );
		}

		float GetFriction()
		{
			return mRigidBody->getFriction();
		}

		void SetFriction( float friction )
		{
			mRigidBody->setFriction( friction );
		}

		Vector3f GetAngularFactor()
		{
			btVector3 v( mRigidBody->getAngularFactor() );

			return Vector3f( v.x(), v.y(), v.z() );
		}

		void SetAngularFactor( const Vector3f& factor )
		{
			mRigidBody->setAngularFactor( btVector3( factor.x, factor.y, factor.z ));
		}

		Vector3f GetGravity()
		{
			btVector3 v( mRigidBody->getGravity() );

			return Vector3f( v.x(), v.y(), v.z() );
		}

		void SetGravity( const Vector3f& gravity )
		{
			mRigidBody->setGravity( btVector3( gravity.x, gravity.y, gravity.z ));
		}

		//////////////////////////////////

		void ApplyCentralImpulse( const Vector3f& impulse )
		{
			mRigidBody->activate( true );
			mRigidBody->applyCentralImpulse( btVector3( impulse.x, impulse.y, impulse.z ));
		}
	};

	/////////////////////////////////////////////////////////////////////////////
	
	class PhysicsSystemBT : public PhysicsSystem
	{
	private:

		btDefaultCollisionConfiguration*		mConfig;
		btCollisionDispatcher*					mDispatcher;
		btBroadphaseInterface*					mCache;
		btSequentialImpulseConstraintSolver*	mSolver;
		btDiscreteDynamicsWorld*				mWorld;

		bool mIsActive;

	public:

		PhysicsSystemBT()
		{
			mConfig		= new btDefaultCollisionConfiguration();
			mDispatcher	= new btCollisionDispatcher( mConfig );
			mCache		= new btDbvtBroadphase();

			mSolver		= new btSequentialImpulseConstraintSolver;
			mWorld		= new btDiscreteDynamicsWorld( mDispatcher, mCache, mSolver, mConfig );

			mWorld->setGravity( btVector3( 0, -9.81f, 0 ));

			mIsActive = false;
		}

		~PhysicsSystemBT()
		{
			for( int i = mWorld->getNumCollisionObjects()-1; i >=0; --i )
			{
				btCollisionObject* obj = mWorld->getCollisionObjectArray()[ i ];
				btRigidBody* body = btRigidBody::upcast( obj );

				if( body && body->getMotionState() )
					delete body->getMotionState();

				mWorld->removeCollisionObject( obj );
				delete obj;
			}

			SAFE_DELETE( mWorld );
			SAFE_DELETE( mDispatcher );
			SAFE_DELETE( mConfig );
			SAFE_DELETE( mCache );
			SAFE_DELETE( mSolver );
		}

	public:

		void Startup()
		{
			mIsActive = true;
		}

		void Update( float DT )
		{
			if( mIsActive )
			{
				mWorld->stepSimulation( btScalar( DT ));
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

		SpherePhysicsShape* CreateSphere( float radius )
		{
			return new SpherePhysicsShapeBT( radius );
		}

		BoxPhysicsShape* CreateBox( const Vector3f& scale )
		{
			return new BoxPhysicsShapeBT( scale );
		}

		CylinderPhysicsShape* CreateCylinder( const Vector3f& scale )
		{
			return new CylinderPhysicsShapeBT( scale );
		}

		MeshPhysicsShape* CreateMesh( Vector3f* verts, UINT count, const Vector3f& scale )
		{
			return new MeshPhysicsShapeBT( verts, count, scale );
		}

		//////////////////////////////////

		RigidBody* CreateRigidBody( PhysicsShape* shape, const Vector3f& position, const Quatf& orientation, float mass )
		{
			btScalar  tMass( mass );
			btVector3 localInertia( 0, 0, 0 );

			btCollisionShape* btShape = (btCollisionShape*)shape->GetData();

			if( tMass != 0.0f )
				btShape->calculateLocalInertia( tMass, localInertia );

			btTransform transform;
			transform.setIdentity();
			transform.setOrigin( btVector3( position.x, position.y, position.z ));
			transform.setRotation( btQuaternion( orientation.x, orientation.y, orientation.z, orientation.w ));

			btDefaultMotionState* myMotionState = new btDefaultMotionState( transform );

			btRigidBody::btRigidBodyConstructionInfo rbInfo( tMass, myMotionState, btShape, localInertia );

			return new RigidBodyBT( shape, new btRigidBody( rbInfo ));
		}

		void AddRigidBody( RigidBody* body )
		{
			mWorld->addRigidBody( ((RigidBodyBT*)body)->mRigidBody );
		}

		void RemoveRigidBody( RigidBody* body )
		{
			mWorld->removeCollisionObject( ((RigidBodyBT*)body)->mRigidBody );
			mWorld->removeRigidBody( ((RigidBodyBT*)body)->mRigidBody );
		}
	};

	PhysicsSystem* BuildPhysicsSystemBT()
	{
		return new PhysicsSystemBT();
	}
}
