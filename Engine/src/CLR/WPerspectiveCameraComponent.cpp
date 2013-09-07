#include "WPerspectiveCameraComponent.h"
#include "PerspectiveCameraComponent.h"

namespace Sentinel { namespace Components
{
	WPerspectiveCameraComponent::WPerspectiveCameraComponent()
	{
		mRef = NULL;
	}

	WPerspectiveCameraComponent::WPerspectiveCameraComponent( float windowWidth, float windowHeight )
	{
		mRef = new PerspectiveCameraComponent( windowWidth, windowHeight );
	}

	DEFINE_CAST_COMPONENT( PerspectiveCameraComponent );

	void WPerspectiveCameraComponent::Startup()
	{
		mRef->Startup();
	}

	void WPerspectiveCameraComponent::Update()
	{
		mRef->Update();
	}

	void WPerspectiveCameraComponent::Shutdown()
	{
		mRef->Shutdown();
	}

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
