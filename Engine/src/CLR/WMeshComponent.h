#pragma once

#include "WDrawableComponent.h"
#include "RMesh.h"
#include "WMaterial.h"

using namespace Sentinel::Assets;

namespace Sentinel { namespace Components
{
	public ref class WMeshComponent : public WDrawableComponent
	{
	public:

		WMeshComponent( RMesh^ mesh, WMaterial^ material );
	};
}}
