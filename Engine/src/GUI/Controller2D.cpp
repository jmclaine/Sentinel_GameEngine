#pragma once

#include "GUI\Controller2D.h"
#include "GameObject.h"
#include "Exception.h"

namespace Sentinel {
namespace GUI
{
	Controller2D::Controller2D() :
		mIsOver(false)
	{}

	Controller2D::~Controller2D()
	{}

	void Controller2D::Startup()
	{
		mTransform = mOwner->GetComponent<Component::Transform>();

		if (mTransform == NULL)
			throw AppException("ControllerComponent::Startup()\n" + mOwner->mName + " does not contain TransformComponent");
	}

	void Controller2D::Save(Archive& archive)
	{}

	void Controller2D::Load(Archive& archive)
	{}

	///////////////////////////////

	void Controller2D::Copy(GameComponent* component)
	{
		GameComponent::Copy(component);
	}
}}
