#pragma once

#include "WGameComponent.h"
#include "WVector3f.h"
#include "WQuatf.h"

using namespace Sentinel::Math;

namespace Sentinel { namespace Components
{
	public ref class WTransformComponent : public WGameComponent
	{
	public:

		WTransformComponent();
		WTransformComponent( WTransformComponent% v );
		~WTransformComponent();

		/////////////////////////////////

		WVector3f^		Position();
		WQuatf^			Orientation();
		WVector3f^		Scale();
	};
}}
