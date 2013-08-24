#include "WCameraComponent.h"

namespace Sentinel { namespace Components
{
	WCameraComponent::WCameraComponent()
	{}

	WCameraComponent::WCameraComponent( CameraComponent* camera )
	{
		mRef = camera;
	}

	const WTransformComponent^ WCameraComponent::GetTransform()
	{
		return gcnew WTransformComponent( static_cast< CameraComponent* >(mRef)->GetTransform() );
	}
}}
