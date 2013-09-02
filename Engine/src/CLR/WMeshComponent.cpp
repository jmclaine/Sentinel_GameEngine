#include "WMeshComponent.h"
#include "MeshComponent.h"

namespace Sentinel { namespace Components
{
	WMeshComponent::WMeshComponent( WMesh^ mesh )
	{
		mRef = new MeshComponent( mesh->GetRef() );
	}

	WMeshComponent::WMeshComponent( WGameComponent^ component )
	{
		mRef = component->GetRef();
	}
}}
