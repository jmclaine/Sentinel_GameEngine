#include <vector>

#include "MeshComponent.h"

namespace Sentinel
{
	DEFINE_SERIAL_REGISTER( MeshComponent );
	DEFINE_SERIAL_CLONE( MeshComponent );

	MeshComponent::MeshComponent()
	{}

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

	///////////////////////////////////

	void MeshComponent::Save( Archive& archive )
	{
		mSerialRegistry.Save( archive );
	}

	void MeshComponent::Load( Archive& archive )
	{}
}
