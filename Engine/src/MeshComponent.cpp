#include <vector>

#include "MeshComponent.h"

namespace Sentinel
{
	MeshComponent::MeshComponent( std::shared_ptr< Mesh > mesh, const Material& material )
	{
		mMesh		= mesh;
		mMaterial	= material;
	}

	void MeshComponent::Startup()
	{
		DrawableComponent::Startup();
	}

	void MeshComponent::Update()
	{
		DrawableComponent::Update();

		mMesh->SetMaterial( mMaterial );

		mMesh->SetWorldTransform( mMatrixTranslation * mMatrixRotation * mMatrixScale );

		mMesh->Draw();
	}

	void MeshComponent::Shutdown()
	{
		mMesh.reset();
	}
}
