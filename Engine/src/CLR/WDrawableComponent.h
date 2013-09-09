#pragma once

#include "DrawableComponent.h"
#include "WGameObject.h"
#include "WMaterial.h"

using namespace Sentinel::Assets;

namespace Sentinel { namespace Components
{
	public ref class WDrawableComponent : public WGameComponent
	{
	protected:

		WDrawableComponent();

	public:

		WDrawableComponent( DrawableComponent* component );

		DECLARE_CAST_COMPONENT( DrawableComponent );
	};
}}
