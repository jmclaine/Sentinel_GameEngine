#pragma once

#include "btBulletDynamicsCommon.h"

#include "Mesh.h"
#include "Singleton.h"
#include "Vector3f.h"
#include "Quatf.h"

namespace Sentinel
{
	class PhysicsSystem : public Singleton< PhysicsSystem >
	{
	private:

		btDefaultCollisionConfiguration*					mConfig;
		btCollisionDispatcher*								mDispatcher;
		btBroadphaseInterface*								mCache;
		btSequentialImpulseConstraintSolver*				mSolver;
		btDiscreteDynamicsWorld*							mWorld;

		btAlignedObjectArray< btCollisionShape* >			mShape;
		btAlignedObjectArray< btTriangleIndexVertexArray* > mShapeData;

	public:

		PhysicsSystem();

		void			Startup();

		void			Update();

		void			Shutdown();

		//////////////////////////////////

		btRigidBody*	CreateSphere( const Vector3f& position, const Quatf& orientation, float radius, float mass );

		btRigidBody*	CreateBox( const Vector3f& position, const Quatf& orientation, const Vector3f& scale, float mass );

		btRigidBody*	CreateCylinder( const Vector3f& position, const Quatf& orientation, const Vector3f& scale, float mass );

		btRigidBody*	CreateMesh( const Vector3f& position, const Quatf& orientation, const Vector3f& scale, Mesh* mesh, float mass );

	private:

		btRigidBody*	CreateObject( btCollisionShape* shape, const btVector3& position, const btQuaternion& orientation, btScalar mass );
	};
}
