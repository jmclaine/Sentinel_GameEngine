#include "Component/Controller.h"
#include "GameObject.h"
#include "Component/Physics.h"
#include "Component/Transform.h"

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

		if (mPhysics == NULL)
			throw AppException("Controller::Startup()\n" + std::string(mOwner->mName) + " does not contain Physics");

		///////////////////////////

		mTransform = mOwner->mTransform;

		if (mTransform == NULL)
			throw AppException("Controller::Startup()\n" + std::string(mOwner->mName) + " does not contain Transform");
	}

	void Controller::Update()
	{}

	void Controller::Shutdown()
	{
		mPhysics = NULL;
		mTransform = NULL;
	}

	//////////////////////////////

	void Controller::Execute()
	{}

	void Controller::SetOwner(GameObject* owner)
	{
		GameComponent::SetOwner(owner);

		if (owner->mController == NULL)
			owner->mController = this;
	}
}}
