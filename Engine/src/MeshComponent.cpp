#include <vector>

#include "MeshComponent.h"
#include "MeshManager.h"

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
		_ASSERT( mMesh );

		DrawableComponent::Startup();
	}

	void MeshComponent::Update()
	{
		DrawableComponent::Update();

		if( mTransform )
		{
			mMesh->mMaterial = mMaterial;

			mMesh->mMatrixWorld = mTransform->GetMatrixWorld();

			mMesh->Draw();
		}
	}

	void MeshComponent::Shutdown()
	{
		DrawableComponent::Shutdown();
	}

	///////////////////////////////////

	void MeshComponent::Save( Archive& archive )
	{
		mSerialRegistry.Save( archive );

		GameComponent::Save( archive );

		archive.Write( &MeshManager::Inst()->Get( mMesh ));
		archive.Write( mMaterial.Ptr(), ar_sizeof( mMaterial ));
	}

	void MeshComponent::Load( Archive& archive )
	{
		GameComponent::Load( archive );

		std::string name;
		archive.Read( &name );

		mMesh = MeshManager::Inst()->Get( name );

		archive.Read( mMaterial.Ptr(), ar_sizeof( mMaterial ));
	}
}
