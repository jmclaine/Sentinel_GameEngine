#include "PhysicsSystem.h"
#include "Renderer.h"
#include "Timing.h"
#include "Util.h"

namespace Sentinel
{
	PhysicsSystem::PhysicsSystem() :
		mConfig( NULL ), mDispatcher( NULL ), mCache( NULL ), mSolver( NULL ), mWorld( NULL )
	{}

	void PhysicsSystem::Startup()
	{
		mConfig		= new btDefaultCollisionConfiguration();
		mDispatcher	= new btCollisionDispatcher( mConfig );
		mCache		= new btDbvtBroadphase();

		mSolver		= new btSequentialImpulseConstraintSolver;
		mWorld		= new btDiscreteDynamicsWorld( mDispatcher, mCache, mSolver, mConfig );

		mWorld->setGravity( btVector3( 0, -9.81f, 0 ));
	}

	void PhysicsSystem::Update()
	{
		float DT = Timing::Inst()->DeltaTime();

		while( DT > 0 )
		{
			mWorld->stepSimulation( btScalar( DT ));

			DT -= (float)Timing::DESIRED_FRAME_RATE;
		}
	}

	void PhysicsSystem::Shutdown()
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

#define CREATE_OBJECT()\
	return CreateObject( shape, \
						 btVector3( position.X(), position.Y(), position.Z() ), \
						 btQuaternion( orientation.X(), orientation.Y(), orientation.Z(), orientation.W() ), \
						 mass );

	btRigidBody* PhysicsSystem::CreateSphere( const Vector3f& position, const Quatf& orientation, float radius, float mass )
	{
		btCollisionShape* shape = new btSphereShape( btScalar( radius ));

		CREATE_OBJECT();
	}

	btRigidBody* PhysicsSystem::CreateBox( const Vector3f& position, const Quatf& orientation, const Vector3f& scale, float mass )
	{
		btCollisionShape* shape = new btBoxShape( btVector3( scale.X(), scale.Y(), scale.Z() ));

		CREATE_OBJECT();
	}

	btRigidBody* PhysicsSystem::CreateCylinder( const Vector3f& position, const Quatf& orientation, const Vector3f& scale, float mass )
	{
		// scale.Z() is unused
		btCollisionShape* shape = new btCylinderShape( btVector3( scale.X(), scale.Y()*0.5f, scale.X() ));

		CREATE_OBJECT();
	}

	btRigidBody* PhysicsSystem::CreateMesh( const Vector3f& position, const Quatf& orientation, const Vector3f& scale, Mesh* mesh, float mass )
	{
		Buffer* vbo = mesh->GetVBO();
		Buffer* ibo = mesh->GetIBO();
		
		// Store only the vertex positions.
		//
		UINT   count		= vbo->mCount;
		UCHAR* vboData		= (UCHAR*)vbo->Lock();
		UCHAR* vboCopy		= new UCHAR[ count * sizeof(Vector3f) ];
		UCHAR* vboCopyData	= vboCopy;

		for( UINT x = 0; x < count; ++x )
		{
			// Must be scaled down a little to compensate for bullet shape object numerical error.
			//
			*(Vector3f*)vboCopyData = *(Vector3f*)vboData * 0.935f;

			vboCopyData += sizeof(Vector3f);
			vboData += vbo->mStride;
		}
		vbo->Unlock();

		UCHAR* iboCopy = new UCHAR[ ibo->mSize ];
		memcpy( iboCopy, (UCHAR*)ibo->Lock(), ibo->mSize );
		ibo->Unlock();

		btIndexedMesh iMesh;
		iMesh.m_vertexType			= PHY_FLOAT;
		iMesh.m_numVertices			= vbo->mCount;
		iMesh.m_vertexBase			= vboCopy;
		iMesh.m_vertexStride		= sizeof(Vector3f);

		iMesh.m_indexType			= PHY_INTEGER;
		iMesh.m_numTriangles		= ibo->mCount;
		iMesh.m_triangleIndexBase	= iboCopy;
		iMesh.m_triangleIndexStride = ibo->mStride;

		btTriangleIndexVertexArray* tiva = new btTriangleIndexVertexArray();

		tiva->addIndexedMesh( iMesh, PHY_UCHAR );
		tiva->setScaling( btVector3( scale.X(), scale.Y(), scale.Z() ));

		mShapeData.push_back( tiva );

		btConvexTriangleMeshShape* shape = new btConvexTriangleMeshShape( tiva, true );

		CREATE_OBJECT();
	}

	btRigidBody* PhysicsSystem::CreateObject( btCollisionShape* shape, const btVector3& position, const btQuaternion& orientation, btScalar mass )
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

		btRigidBody* body = new btRigidBody( rbInfo );

		mWorld->addRigidBody( body );

		return body;
	}
}
