#include "WControllerComponent.h"

namespace Sentinel { namespace Wrapped
{
	WControllerComponent::WControllerComponent()
	{
		mRef = NULL;
	}

	WControllerComponent::WControllerComponent( ControllerComponent* component )
	{
		mRef = component;
	}

	DEFINE_CAST_COMPONENT( ControllerComponent );
}}
