#include "Component/Physics.h"
#include "Component/Transform.h"
#include "PhysicsSystem.h"
#include "MeshManager.h"
#include "GameObject.h"
#include "GameWorld.h"
#include "Archive.h"
#include "Vector3.h"
#include "Quaternion.h"
#include "Exception.h"

namespace Sentinel {
namespace Component
{
	DEFINE_SERIAL_REGISTER(Physics);
	DEFINE_SERIAL_CLONE(Physics);

	/////////////////////////////////

	Physics::Physics()
	{
		mTransform = NULL;
		mRigidBody = NULL;
	}

	Physics::Physics(RigidBody* body)
	{
		mTransform = NULL;
		mRigidBody = body;
	}

	Physics::~Physics()
	{
		SAFE_DELETE(mRigidBody);
	}

	void Physics::Startup()
	{
		_ASSERT(mOwner);
		_ASSERT(mOwner->GetWorld());
		_ASSERT(mOwner->GetWorld()->mPhysicsSystem);

		mTransform = mOwner->mTransform;

		if (mTransform == NULL)
			throw AppException("Physics::Startup()\n" + std::string(mOwner->mName) + " does not contain Transform");

		if (mRigidBody == NULL)
			throw AppException("Physics::Startup()\n" + std::string(mOwner->mName) + " does not contain RigidBody");

		mOwner->GetWorld()->mPhysicsSystem->AddRigidBody(mRigidBody);
	}

	void Physics::Shutdown()
	{
		mTransform = NULL;

		mOwner->GetWorld()->mPhysicsSystem->RemoveRigidBody(mRigidBody);
	}

	/////////////////////////////////

	void Physics::Execute()
	{
		//if (mOwner->GetParent() == NULL)
		{
			mTransform->mPosition = mRigidBody->GetPosition();
			mTransform->mOrientation = mRigidBody->GetOrientation();
			//mTransform->mScale = mRigidBody->GetScale();
		}
	}

	void Physics::SetOwner(GameObject* owner)
	{
		GameComponent::SetOwner(owner);

		mOwner->mPhysics = this;
	}

	RigidBody* Physics::GetRigidBody()
	{
		return mRigidBody;
	}

	void Physics::SetRigidBody(RigidBody* body)
	{
		_ASSERT(body);

		if (mRigidBody)
			mOwner->GetWorld()->mPhysicsSystem->RemoveRigidBody(mRigidBody);

		mRigidBody = body;
	}

	void Physics::Save(Archive& archive)
	{
		SERIAL_REGISTER.Save(archive);

		GameComponent::Save(archive);

		mRigidBody->GetShape()->Save(archive);

		float mass = mRigidBody->GetMass();
		archive.Write(&mass);

		Vector3 pos = mRigidBody->GetPosition();
		archive.Write(pos.Ptr(), ar_sizeof(pos));

		Quaternion rot = mRigidBody->GetOrientation();
		archive.Write(rot.Ptr(), ar_sizeof(rot));

		int flags = mRigidBody->GetFlags();
		archive.Write(&flags);

		float value = mRigidBody->GetLinearDamping();
		archive.Write(&value);

		value = mRigidBody->GetAngularDamping();
		archive.Write(&value);

		value = mRigidBody->GetRestitution();
		archive.Write(&value);

		value = mRigidBody->GetFriction();
		archive.Write(&value);

		Vector3 v = mRigidBody->GetAngularFactor();
		archive.Write(v.Ptr(), ar_sizeof(v));

		v = mRigidBody->GetGravity();
		archive.Write(v.Ptr(), ar_sizeof(v));
	}

	void Physics::Load(Archive& archive)
	{
		GameComponent::Load(archive);

		PhysicsSystem* physics = mOwner->GetWorld()->mPhysicsSystem;

		PhysicsShape* shape = (PhysicsShape*)SerialMemberFunctionRegister<PhysicsSystem>::Load(physics, archive);
		shape->Load(archive);

		float mass;
		archive.Read(&mass);

		Vector3 pos;
		archive.Read(pos.Ptr(), ar_sizeof(pos));

		Quaternion rot;
		archive.Read(rot.Ptr(), ar_sizeof(rot));

		mRigidBody = physics->CreateRigidBody(shape, pos, rot, mass);

		int flags;
		archive.Read(&flags);
		mRigidBody->SetFlags(flags);

		float linearDamping;
		archive.Read(&linearDamping);
		float angularDamping;
		archive.Read(&angularDamping);
		mRigidBody->SetDamping(linearDamping, angularDamping);

		float rest;
		archive.Read(&rest);
		mRigidBody->SetRestitution(rest);

		float friction;
		archive.Read(&friction);
		mRigidBody->SetFriction(friction);

		Vector3 angularFactor;
		archive.Read(angularFactor.Ptr(), ar_sizeof(angularFactor));
		mRigidBody->SetAngularFactor(angularFactor);

		Vector3 gravity;
		archive.Read(gravity.Ptr(), ar_sizeof(gravity));
		mRigidBody->SetGravity(gravity);
	}

	/////////////////////////////////

	GameComponent* Physics::Copy()
	{
		Physics* physics = new Physics();

		GameComponent::Copy(physics);

		PhysicsSystem* phySys = mOwner->GetWorld()->mPhysicsSystem;
		PhysicsShape* shape = mRigidBody->GetShape()->Copy(phySys);
		RigidBody* body = phySys->CreateRigidBody(shape, mRigidBody->GetPosition(), mRigidBody->GetOrientation(), mRigidBody->GetMass());

		body->SetFlags(mRigidBody->GetFlags());
		body->SetDamping(mRigidBody->GetLinearDamping(), mRigidBody->GetAngularDamping());
		body->SetRestitution(mRigidBody->GetRestitution());
		body->SetFriction(mRigidBody->GetFriction());
		body->SetAngularFactor(mRigidBody->GetAngularFactor());
		body->SetGravity(mRigidBody->GetGravity());

		return physics;
	}
}}
