#pragma once

#include "LightComponent.h"
#include "WGameComponent.h"
#include "WTransformComponent.h"
#include "WColor.h"
#include "WVector4f.h"

using namespace Sentinel::Math;
using namespace Sentinel::Assets;

namespace Sentinel { namespace Components
{
	public ref class WLightComponent : public WGameComponent
	{
	public:

		WLightComponent();
		WLightComponent( LightComponent* light );

		const WTransformComponent^	GetTransform();

		void			Startup();

		void			Update();

		void			Shutdown();

		//////////////////////////////

		RColorRGBA^		Color();
		RVector4f^		Attenuation();
	};
}}
