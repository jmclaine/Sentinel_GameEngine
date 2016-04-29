#include "Component/Trigger.h"
#include "GameObject.h"
#include "Component/Physics.h"
#include "Exception.h"

namespace Sentinel
{
	namespace Component
	{
		SerialRegister Trigger::SERIAL_REGISTER("Trigger", Trigger::Clone);
		Serializable* Trigger::Clone() { return new Trigger(); }

		Trigger::Trigger() :
			mPhysics(nullptr)
		{ }

		Trigger::~Trigger()
		{ }

		void Trigger::Startup()
		{
			mPhysics = mOwner->GetComponent<Physics>();

			if (mPhysics == nullptr)
				throw AppException("Trigger::Startup()\n" + std::string(mOwner->mName) + " does not contain PhysicsComponent");
		}

		void Trigger::Update()
		{ }

		void Trigger::Shutdown()
		{
			mPhysics = nullptr;
		}

		/////////////////////////////////

		void Trigger::Save(Archive& archive)
		{
			SERIAL_REGISTER.Save(archive);

			GameComponent::Save(archive);
		}

		void Trigger::Load(Archive& archive)
		{
			GameComponent::Load(archive);
		}

		/////////////////////////////////

		GameComponent* Trigger::Copy()
		{
			Trigger* trigger = new Trigger();

			Copy(trigger);

			return trigger;
		}

		void Trigger::Copy(GameComponent* component)
		{
			GameComponent::Copy(component);
		}
	}
}
