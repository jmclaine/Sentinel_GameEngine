#include "WMeshComponent.h"
#include "MeshComponent.h"

namespace Sentinel { namespace Components
{
	WMeshComponent::WMeshComponent( WMesh^ mesh )
	{
		mRef = new MeshComponent( mesh->GetRef() );
	}

	WMeshComponent::WMeshComponent( MeshComponent* component )
	{
		mRef = component;
	}

	DEFINE_CAST_COMPONENT( MeshComponent );

	DEFINE_PROPERTY_PS( MeshComponent, Mesh,	 Mesh );
	DEFINE_PROPERTY_RS( MeshComponent, Material, Material );
}}
