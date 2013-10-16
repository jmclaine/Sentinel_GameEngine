#include "WPerspectiveCameraComponent.h"
#include "PerspectiveCameraComponent.h"

namespace Sentinel { namespace Wrapped
{
	WPerspectiveCameraComponent::WPerspectiveCameraComponent()
	{
		mRef = NULL;
	}

	WPerspectiveCameraComponent::WPerspectiveCameraComponent( float windowWidth, float windowHeight )
	{
		mRef = new PerspectiveCameraComponent( windowWidth, windowHeight );
	}

	WPerspectiveCameraComponent::WPerspectiveCameraComponent( PerspectiveCameraComponent* component )
	{
		mRef = component;
	}

	DEFINE_CAST_COMPONENT( PerspectiveCameraComponent );

	//////////////////////////////

	void WPerspectiveCameraComponent::Set( float windowWidth, float windowHeight )
	{
		static_cast< PerspectiveCameraComponent*>(mRef)->Set( windowWidth, windowHeight );
	}

	void WPerspectiveCameraComponent::Set( float windowWidth, float windowHeight, float nearZ, float farZ, float FOV )
	{
		static_cast< PerspectiveCameraComponent*>(mRef)->Set( windowWidth, windowHeight, nearZ, farZ, FOV );
	}
}}
