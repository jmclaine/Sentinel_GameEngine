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

	const WTransformComponent^ WLightComponent::GetTransform()
	{
		return gcnew WTransformComponent( static_cast< LightComponent* >(mRef)->GetTransform() );
	}

	void WLightComponent::Startup()
	{
		static_cast< LightComponent* >(mRef)->Startup();
	}

	void WLightComponent::Update()
	{
		static_cast< LightComponent* >(mRef)->Update();
	}

	void WLightComponent::Shutdown()
	{
		static_cast< LightComponent* >(mRef)->Shutdown();
	}

	RColorRGBA^ WLightComponent::Color()
	{
		return gcnew RColorRGBA( &static_cast< LightComponent* >(mRef)->mColor );
	}

	RVector4f^ WLightComponent::Attenuation()
	{
		return gcnew RVector4f( &static_cast< LightComponent* >(mRef)->mAttenuation );
	}
}}
