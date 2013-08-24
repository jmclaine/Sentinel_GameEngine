#include "WMeshComponent.h"
#include "MeshComponent.h"

namespace Sentinel { namespace Components
{
	WMeshComponent::WMeshComponent( RMesh^ mesh, WMaterial^ material )
	{
		mRef = new MeshComponent( mesh->GetRef(), *material->GetRef() );
	}
}}
