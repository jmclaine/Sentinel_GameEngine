#include "TransformComponent.h"

namespace Sentinel
{
	TransformComponent::TransformComponent()
	{
		mComponentType = COMPONENT_TRANSFORM;

		mScale = Vector3f( 1, 1, 1 );
	}

	void TransformComponent::Startup()
	{}

	void TransformComponent::Update()
	{}

	void TransformComponent::Shutdown()
	{}
}
