#pragma once

#include "WGameObject.h"
#include "WMaterial.h"

using namespace Sentinel::Assets;

namespace Sentinel { namespace Components
{
	public ref class WDrawableComponent : public WGameComponent
	{
	public:

		DECLARE_PROPERTY( WMaterial^, Material );
	};
}}
