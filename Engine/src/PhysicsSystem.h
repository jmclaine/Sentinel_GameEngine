#pragma once

#include "btBulletDynamicsCommon.h"

#include "Mesh.h"
#include "MathLib.h"
#include "Singleton.h"

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

		void Startup();

		void Update();

		void Shutdown();

		//////////////////////////////////

		btRigidBody* CreateSphere( const vec3f& position, const quatf& orientation, float radius, float mass );

		btRigidBody* CreateBox( const vec3f& position, const quatf& orientation, const vec3f& scale, float mass );

		btRigidBody* CreateCylinder( const vec3f& position, const quatf& orientation, const vec3f& scale, float mass );

		btRigidBody* CreateMesh( const vec3f& position, const quatf& orientation, const vec3f& scale, Mesh* mesh, float mass );

	private:

		btRigidBody* CreateObject( btCollisionShape* shape, const btVector3& position, const btQuaternion& orientation, btScalar mass );
	};
}
