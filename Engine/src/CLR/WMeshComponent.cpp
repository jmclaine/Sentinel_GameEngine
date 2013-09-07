#include "WMeshComponent.h"
#include "MeshComponent.h"

namespace Sentinel { namespace Components
{
	WMeshComponent::WMeshComponent( WMesh^ mesh )
	{
		mRef = new MeshComponent( mesh->GetRef() );
	}

	DEFINE_CAST_COMPONENT( MeshComponent );
}}
