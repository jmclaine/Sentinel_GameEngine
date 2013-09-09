#include <vector>

#include "MeshComponent.h"

namespace Sentinel
{
	MeshComponent::MeshComponent( std::shared_ptr< Mesh > mesh )
	{
		mMesh = mesh;

		mMaterial = mMesh->mMaterial;
	}

	void MeshComponent::Startup()
	{
		DrawableComponent::Startup();
	}

	void MeshComponent::Update()
	{
		DrawableComponent::Update();

		mMesh->mMaterial = mMaterial;

		mMesh->mMatrixWorld = mTransform->GetMatrixWorld();

		mMesh->Draw();
	}

	void MeshComponent::Shutdown()
	{
		mMesh.reset();
	}
}
