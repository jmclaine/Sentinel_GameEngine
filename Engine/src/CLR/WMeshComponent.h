#pragma once

#include "WDrawableComponent.h"
#include "WMesh.h"

using namespace Sentinel::Assets;

namespace Sentinel { namespace Components
{
	public ref class WMeshComponent : public WDrawableComponent
	{
	public:

		WMeshComponent( WMesh^ mesh );
		WMeshComponent( WGameComponent^ component );
	};
}}
