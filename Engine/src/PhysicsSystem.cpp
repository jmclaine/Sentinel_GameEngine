#include "PhysicsSystem.h"

namespace Sentinel
{
	RigidBody::RigidBody() :
		mShape(nullptr)
	{ }

	RigidBody::~RigidBody()
	{
		delete mShape;
	}

	////////////////////////////////////////////////////////////////////

	DEFINE_SERIAL_MEMBER_FUNCTION(PhysicsSystem, CreateSphere);
	DEFINE_SERIAL_MEMBER_FUNCTION(PhysicsSystem, CreateBox);
	DEFINE_SERIAL_MEMBER_FUNCTION(PhysicsSystem, CreateCylinder);
	DEFINE_SERIAL_MEMBER_FUNCTION(PhysicsSystem, CreateMesh);
}
