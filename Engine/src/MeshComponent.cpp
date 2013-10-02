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

	MeshComponent::~MeshComponent()
	{}

	void MeshComponent::Startup()
	{
		DrawableComponent::Startup();
	}

	void MeshComponent::Update()
	{
		mMesh->mMaterial = mMaterial;

		mMesh->mMatrixWorld = mTransform->GetMatrixWorld();

		mMesh->Draw();
	}

	void MeshComponent::Shutdown()
	{}

	///////////////////////////////////

	void MeshComponent::Save( Archive& archive )
	{
		mSerialRegistry.Save( archive );
	}

	void MeshComponent::Load( Archive& archive )
	{}
}
