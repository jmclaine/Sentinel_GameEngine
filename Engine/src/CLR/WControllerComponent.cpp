#include "WControllerComponent.h"

namespace Sentinel { namespace Components
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
