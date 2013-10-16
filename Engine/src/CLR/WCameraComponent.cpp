#include "WCameraComponent.h"

namespace Sentinel { namespace Wrapped
{
	WCameraComponent::WCameraComponent()
	{}

	WCameraComponent::WCameraComponent( CameraComponent* camera )
	{
		mRef = camera;
	}

	DEFINE_CAST_COMPONENT( CameraComponent );

	const WTransformComponent^ WCameraComponent::GetTransform()
	{
		return gcnew WTransformComponent( static_cast< CameraComponent* >(mRef)->GetTransform() );
	}

	DEFINE_PROPERTY_RS( CameraComponent, Matrix4f, MatrixView );
	DEFINE_PROPERTY_RS( CameraComponent, Matrix4f, MatrixProjection );
	DEFINE_PROPERTY_RS( CameraComponent, Matrix4f, MatrixFinal );
}}
