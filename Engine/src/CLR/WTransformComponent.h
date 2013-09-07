#pragma once

#include "TransformComponent.h"
#include "WGameComponent.h"
#include "WMatrix4f.h"
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
		
		DECLARE_CAST_COMPONENT( WTransformComponent );

		/////////////////////////////////

		DECLARE_PROPERTY( WVector3f^,	Position );
		DECLARE_PROPERTY( WQuatf^,		Orientation );
		DECLARE_PROPERTY( WVector3f^,	Scale );

		/////////////////////////////////

		WMatrix4f^	GetMatrixWorld();
	};
}}
