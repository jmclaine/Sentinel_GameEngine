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

namespace Sentinel
{
	namespace Component
	{
		SerialRegister Physics::SERIAL_REGISTER("Physics", Physics::Clone);
		Serializable* Physics::Clone() { return new Physics(); }

		/////////////////////////////////

		Physics::Physics()
		{
			mTransform = nullptr;
			mRigidBody = nullptr;
		}

		Physics::Physics(RigidBody* body)
		{
			mTransform = nullptr;
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

			if (mTransform == nullptr)
				throw AppException("Physics::Startup()\n" + std::string(mOwner->mName) + " does not contain Transform");

			if (mRigidBody == nullptr)
				throw AppException("Physics::Startup()\n" + std::string(mOwner->mName) + " does not contain RigidBody");

			mOwner->GetWorld()->mPhysicsSystem->AddRigidBody(mRigidBody);
		}

		void Physics::Update()
		{
			GameComponent::Update();
		}

		void Physics::Shutdown()
		{
			mTransform = nullptr;

			mOwner->GetWorld()->mPhysicsSystem->RemoveRigidBody(mRigidBody);
		}

		/////////////////////////////////

		void Physics::Execute()
		{
			//if (mOwner->GetParent() == nullptr)
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

			auto physics = mOwner->GetWorld()->mPhysicsSystem.get();

			if (mRigidBody)
				physics->RemoveRigidBody(mRigidBody);

			mRigidBody = body;

			if (mRigidBody)
				physics->AddRigidBody(mRigidBody);
		}

		void Physics::Save(Archive& archive)
		{
			SERIAL_REGISTER.Save(archive);

			GameComponent::Save(archive);

			mRigidBody->GetShape()->Save(archive);

			float mass = mRigidBody->GetMass();
			archive.Write(&mass);

			Vector3 pos = mRigidBody->GetPosition();
			archive.Write(&pos);

			Quaternion rot = mRigidBody->GetOrientation();
			archive.Write(&rot);

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
			archive.Write(&v);

			v = mRigidBody->GetGravity();
			archive.Write(&v);
		}

		void Physics::Load(Archive& archive)
		{
			GameComponent::Load(archive);

			PhysicsSystem* physics = mOwner->GetWorld()->mPhysicsSystem.get();

			PhysicsShape* shape = (PhysicsShape*)SerialMemberFunctionRegister<PhysicsSystem>::Load(physics, archive);
			shape->Load(archive);

			float mass;
			archive.Read(&mass);

			Vector3 pos;
			archive.Read(&pos);

			Quaternion rot;
			archive.Read(&rot);

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
			archive.Read(&angularFactor);
			mRigidBody->SetAngularFactor(angularFactor);

			Vector3 gravity;
			archive.Read(&gravity);
			mRigidBody->SetGravity(gravity);
		}

		/////////////////////////////////

		GameComponent* Physics::Copy()
		{
			Physics* physics = new Physics();

			Copy(physics);

			return physics;
		}

		void Physics::Copy(GameComponent* component)
		{
			GameComponent::Copy(component);

			PhysicsSystem* phySys = mOwner->GetWorld()->mPhysicsSystem.get();
			PhysicsShape* shape = mRigidBody->GetShape()->Copy(phySys);
			RigidBody* body = phySys->CreateRigidBody(shape, mRigidBody->GetPosition(), mRigidBody->GetOrientation(), mRigidBody->GetMass());

			body->SetFlags(mRigidBody->GetFlags());
			body->SetDamping(mRigidBody->GetLinearDamping(), mRigidBody->GetAngularDamping());
			body->SetRestitution(mRigidBody->GetRestitution());
			body->SetFriction(mRigidBody->GetFriction());
			body->SetAngularFactor(mRigidBody->GetAngularFactor());
			body->SetGravity(mRigidBody->GetGravity());

			auto physics = static_cast<Physics*>(component);
			physics->mRigidBody = body;
		}
	}
}
