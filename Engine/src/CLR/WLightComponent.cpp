#include "WLightComponent.h"

namespace Sentinel { namespace Components
{
	WLightComponent::WLightComponent()
	{
		mRef = new LightComponent();
	}

	WLightComponent::WLightComponent( LightComponent* light )
	{
		mRef = light;
	}

	DEFINE_CAST_COMPONENT( LightComponent );

	const WTransformComponent^ WLightComponent::GetTransform()
	{
		return gcnew WTransformComponent( static_cast< LightComponent* >(mRef)->GetTransform() );
	}

	//////////////////////////////

	DEFINE_PROPERTY_RS( LightComponent, ColorRGBA, Color );
	DEFINE_PROPERTY_RS( LightComponent, Vector4f,  Attenuation );
}}
