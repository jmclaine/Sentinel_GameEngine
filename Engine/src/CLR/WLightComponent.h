#pragma once

#include "LightComponent.h"
#include "WGameComponent.h"
#include "WTransformComponent.h"
#include "WColorRGBA.h"
#include "WVector4f.h"

using namespace Sentinel::Wrapped;
using namespace Sentinel::Wrapped;

namespace Sentinel { namespace Wrapped
{
	public ref class WLightComponent : public WGameComponent
	{
	public:

		WLightComponent();
		WLightComponent( LightComponent* light );

		DECLARE_CAST_COMPONENT( LightComponent );

		const WTransformComponent^	GetTransform();

		//////////////////////////////

		DECLARE_PROPERTY( WColorRGBA^,	Color );
		DECLARE_PROPERTY( WVector4f^,	Attenuation );
	};
}}
