#include <vector>

#include "MeshComponent.h"

namespace Sentinel
{
	MeshComponent::MeshComponent( std::shared_ptr< Mesh > mesh )
	{
		mMesh = mesh;
	}

	void MeshComponent::Startup()
	{
		DrawableComponent::Startup();
	}

	void MeshComponent::Update()
	{
		DrawableComponent::Update();

		mMesh->SetMaterial( mMaterial );

		mMesh->SetWorldTransform( mTransform->GetMatrixWorld() );

		mMesh->Draw();
	}

	void MeshComponent::Shutdown()
	{
		mMesh.reset();
	}
}
