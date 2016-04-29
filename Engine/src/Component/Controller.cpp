#include "Component/Controller.h"
#include "Component/Physics.h"
#include "Component/Transform.h"
#include "GameObject.h"
#include "Exception.h"

namespace Sentinel {
namespace Component
{
	Controller::Controller()
	{
		Shutdown();
	}

	void Controller::Startup()
	{
		mPhysics = mOwner->mPhysics;

		if (mPhysics == nullptr)
			throw AppException("Controller::Startup()\n" + std::string(mOwner->mName) + " does not contain Physics");

		///////////////////////////

		mTransform = mOwner->mTransform;

		if (mTransform == nullptr)
			throw AppException("Controller::Startup()\n" + std::string(mOwner->mName) + " does not contain Transform");
	}

	void Controller::Shutdown()
	{
		mPhysics = nullptr;
		mTransform = nullptr;
	}

	//////////////////////////////

	void Controller::Execute()
	{ }

	void Controller::SetOwner(GameObject* owner)
	{
		GameComponent::SetOwner(owner);

		if (owner->mController == nullptr)
			owner->mController = this;
	}
}}
