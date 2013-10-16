#pragma once

#include "WPlayerControllerComponent.h"

namespace Sentinel { namespace Wrapped
{
	WPlayerControllerComponent::WPlayerControllerComponent()
	{
		mRef = new PlayerControllerComponent();
	}

	WPlayerControllerComponent::WPlayerControllerComponent( PlayerControllerComponent* component )
	{
		mRef = component;
	}

	DEFINE_CAST_COMPONENT( PlayerControllerComponent );

	//////////////////////////////

	DEFINE_PROPERTY_MS( PlayerControllerComponent, float, Speed );
	DEFINE_PROPERTY_MS( PlayerControllerComponent, float, AngularSpeed );
}}
