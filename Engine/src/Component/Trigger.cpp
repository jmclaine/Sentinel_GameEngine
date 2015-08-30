#include "Component/Trigger.h"
#include "GameObject.h"
#include "Component/Physics.h"
#include "Exception.h"

namespace Sentinel {
namespace Component
{
	DEFINE_SERIAL_REGISTER(Trigger);
	DEFINE_SERIAL_CLONE(Trigger);

	Trigger::Trigger() :
		mPhysics(NULL)
	{}

	Trigger::~Trigger()
	{}

	void Trigger::Startup()
	{
		mPhysics = mOwner->GetComponent<Physics>();

		if (mPhysics == NULL)
			throw AppException("Trigger::Startup()\n" + std::string(mOwner->mName) + " does not contain PhysicsComponent");
	}

	void Trigger::Update()
	{}

	void Trigger::Shutdown()
	{
		mPhysics = NULL;
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
}}
