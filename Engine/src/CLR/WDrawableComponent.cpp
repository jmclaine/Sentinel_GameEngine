#include "WDrawableComponent.h"
#include "DrawableComponent.h"

namespace Sentinel { namespace Wrapped
{
	WDrawableComponent::WDrawableComponent()
	{
		mRef = NULL;
	}

	WDrawableComponent::WDrawableComponent( DrawableComponent* component )
	{
		mRef = component;
	}

	DEFINE_CAST_COMPONENT( DrawableComponent );
}}
