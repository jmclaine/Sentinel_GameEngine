#include "WDrawableComponent.h"
#include "DrawableComponent.h"

namespace Sentinel { namespace Components
{
	WDrawableComponent::WDrawableComponent()
	{
		mRef = NULL;
	}

	DEFINE_CAST_COMPONENT( DrawableComponent );

	DEFINE_PROPERTY_R( DrawableComponent, Material, Material );
}}
