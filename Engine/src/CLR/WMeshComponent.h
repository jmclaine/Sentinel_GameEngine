#pragma once

#include "MeshComponent.h"
#include "WDrawableComponent.h"
#include "WMesh.h"

using namespace Sentinel::Wrapped;

namespace Sentinel { namespace Wrapped
{
	public ref class WMeshComponent : public WDrawableComponent
	{
	public:

		WMeshComponent( WMesh^ mesh );
		WMeshComponent( MeshComponent* component );

		DECLARE_CAST_COMPONENT( MeshComponent );

		DECLARE_PROPERTY( WMesh^,		Mesh );
		DECLARE_PROPERTY( WMaterial^,	Material );
	};
}}
