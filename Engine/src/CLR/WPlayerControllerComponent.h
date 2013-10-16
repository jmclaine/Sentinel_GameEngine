#pragma once

#include "Property.h"
#include "WControllerComponent.h"
#include "PlayerControllerComponent.h"

namespace Sentinel { namespace Wrapped
{
	public ref class WPlayerControllerComponent : public WControllerComponent
	{
	public:

		WPlayerControllerComponent();
		WPlayerControllerComponent( PlayerControllerComponent* component );

		DECLARE_CAST_COMPONENT( PlayerControllerComponent );

		//////////////////////////////

		DECLARE_PROPERTY( float, Speed );
		DECLARE_PROPERTY( float, AngularSpeed );
	};
}}
