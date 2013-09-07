#pragma once

#include "WDrawableComponent.h"
#include "WModel.h"

using namespace Sentinel::Assets;

namespace Sentinel { namespace Components
{
	public ref class WModelComponent : public WDrawableComponent
	{
	public:

		WModelComponent( WModel^ model );

		DECLARE_CAST_COMPONENT( WModelComponent );
	};
}}
