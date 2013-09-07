#pragma once

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

		DECLARE_CAST_COMPONENT( WDrawableComponent );

		DECLARE_PROPERTY( WMaterial^, Material );
	};
}}
