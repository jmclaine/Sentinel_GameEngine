#pragma once

#include "DrawableComponent.h"
#include "WGameObject.h"
#include "WMaterial.h"

using namespace Sentinel::Wrapped;

namespace Sentinel { namespace Wrapped
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
