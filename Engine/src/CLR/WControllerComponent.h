#pragma once

#include "WGameComponent.h"
#include "ControllerComponent.h"

namespace Sentinel { namespace Components
{
	public ref class WControllerComponent : public WGameComponent
	{
	protected:

		WControllerComponent();

	public:

		WControllerComponent( ControllerComponent* component );

		DECLARE_CAST_COMPONENT( ControllerComponent );
	};
}}
