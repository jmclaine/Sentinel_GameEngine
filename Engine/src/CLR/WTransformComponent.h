#pragma once

#include "TransformComponent.h"
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
		WTransformComponent( const TransformComponent* component );
		~WTransformComponent();

		/////////////////////////////////

		RVector3f^		Position();
		RQuatf^			Orientation();
		RVector3f^		Scale();
	};
}}
