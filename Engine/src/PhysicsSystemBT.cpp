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

	// Rigid Body used within the Physics System.
	//
	class RigidBodyBT : public RigidBody
	{
		friend class PhysicsSystemBT;

	private:

		btRigidBody*	mRigidBody;

	public:

		RigidBodyBT( btRigidBody* body )
		{
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

		PhysicsShapeType GetShapeType()
		{
			switch( mRigidBody->getCollisionShape()->getShapeType() )
			{
				case SPHERE_SHAPE_PROXYTYPE:
					return PHYSICS_SPHERE;

				case BOX_SHAPE_PROXYTYPE:
					return PHYSICS_BOX;

				case CYLINDER_SHAPE_PROXYTYPE:
					return PHYSICS_CYLINDER;

				case CONVEX_TRIANGLEMESH_SHAPE_PROXYTYPE:
					return PHYSICS_MESH;

				default:
					return PHYSICS_INVALID;
			}
		}

		void SetShapeType( PhysicsShapeType type )
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
	// Physics System based on Bullet Physics.
	//
	class PhysicsSystemBT : public PhysicsSystem
	{
	private:

		btDefaultCollisionConfiguration*					mConfig;
		btCollisionDispatcher*								mDispatcher;
		btBroadphaseInterface*								mCache;
		btSequentialImpulseConstraintSolver*				mSolver;
		btDiscreteDynamicsWorld*							mWorld;

		btAlignedObjectArray< btCollisionShape* >			mShape;
		btAlignedObjectArray< btTriangleIndexVertexArray* > mShapeData;

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

			TRAVERSE_VECTOR( x, (UINT)mShapeData )
			{
				delete mShapeData[ x ]->getIndexedMeshArray()[ 0 ].m_triangleIndexBase;
				delete mShapeData[ x ]->getIndexedMeshArray()[ 0 ].m_vertexBase;
				delete mShapeData[ x ];
			}
			mShapeData.clear();

			TRAVERSE_VECTOR( x, (UINT)mShape )
				delete mShape[ x ];
			mShape.clear();

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

	#define CREATE_RIGID_BODY()\
		new RigidBodyBT( CreateRigidBody( shape, \
										  btVector3( position.x, position.y, position.z ), \
										  btQuaternion( orientation.x, orientation.y, orientation.z, orientation.w ), \
										  mass ));

		RigidBody* CreateSphere( const Vector3f& position, const Quatf& orientation, float radius, float mass )
		{
			btCollisionShape* shape = new btSphereShape( btScalar( radius ));

			return CREATE_RIGID_BODY();
		}

		RigidBody* CreateBox( const Vector3f& position, const Quatf& orientation, const Vector3f& scale, float mass )
		{
			btCollisionShape* shape = new btBoxShape( btVector3( scale.x, scale.y, scale.z ));

			return CREATE_RIGID_BODY();
		}

		RigidBody* CreateCylinder( const Vector3f& position, const Quatf& orientation, const Vector3f& scale, float mass )
		{
			// scale.Z() is unused
			btCollisionShape* shape = new btCylinderShape( btVector3( scale.x, scale.y*0.5f, scale.x ));

			return CREATE_RIGID_BODY();
		}

		RigidBody* CreateMesh( const Vector3f& position, const Quatf& orientation, const Vector3f& scale, std::shared_ptr< Mesh > mesh, float mass )
		{
			Buffer* vbo = mesh->mVBO;
			Buffer* ibo = mesh->mIBO;
		
			// Store only the vertex positions.
			//
			UINT   count		= vbo->Count();
			UCHAR* vboData		= (UCHAR*)vbo->Lock();
			UCHAR* vboCopy		= new UCHAR[ count * sizeof(Vector3f) ];
			UCHAR* vboCopyData	= vboCopy;

			for( UINT x = 0; x < count; ++x )
			{
				// Must be scaled down a little to compensate for bullet shape object numerical error.
				//
				*(Vector3f*)vboCopyData = *(Vector3f*)vboData * 0.935f;

				vboCopyData += sizeof(Vector3f);
				vboData += vbo->Stride();
			}
			vbo->Unlock();

			UCHAR* iboCopy = new UCHAR[ ibo->Size() ];
			memcpy( iboCopy, (UCHAR*)ibo->Lock(), ibo->Size() );
			ibo->Unlock();

			btIndexedMesh iMesh;
			iMesh.m_vertexType			= PHY_FLOAT;
			iMesh.m_numVertices			= vbo->Count();
			iMesh.m_vertexBase			= vboCopy;
			iMesh.m_vertexStride		= sizeof(Vector3f);

			iMesh.m_indexType			= PHY_INTEGER;
			iMesh.m_numTriangles		= ibo->Count();
			iMesh.m_triangleIndexBase	= iboCopy;
			iMesh.m_triangleIndexStride = ibo->Stride();

			btTriangleIndexVertexArray* tiva = new btTriangleIndexVertexArray();

			tiva->addIndexedMesh( iMesh, PHY_UCHAR );
			tiva->setScaling( btVector3( scale.x, scale.y, scale.z ));

			mShapeData.push_back( tiva );

			btConvexTriangleMeshShape* shape = new btConvexTriangleMeshShape( tiva, true );

			RigidBody* body = CREATE_RIGID_BODY();

			body->mMesh = mesh;

			return body;
		}

		btRigidBody* CreateRigidBody( btCollisionShape* shape, const btVector3& position, const btQuaternion& orientation, btScalar mass )
		{
			mShape.push_back( shape );

			btScalar  tMass( mass );
			btVector3 localInertia( 0, 0, 0 );

			if( tMass != 0.0f )
				shape->calculateLocalInertia( tMass, localInertia );

			btTransform transform;
			transform.setIdentity();
			transform.setOrigin( position );
			transform.setRotation( orientation );

			btDefaultMotionState* myMotionState = new btDefaultMotionState( transform );

			btRigidBody::btRigidBodyConstructionInfo rbInfo( tMass, myMotionState, shape, localInertia );

			return new btRigidBody( rbInfo );
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
