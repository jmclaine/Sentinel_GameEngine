#include <vector>

#include "MeshComponent.h"
#include "MeshManager.h"
#include "Mesh.h"
#include "TransformComponent.h"
#include "GameObject.h"
#include "GameWorld.h"
#include "Archive.h"

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
		_ASSERT( mOwner );
		_ASSERT( mOwner->GetWorld() );
		_ASSERT( mOwner->GetWorld()->mRenderer );

		DrawableComponent::Startup();
	}

	void MeshComponent::Update()
	{
		DrawableComponent::Update();

		if( mTransform )
		{
			mMesh->mMaterial = mMaterial;

			mMesh->mMatrixWorld = mTransform->GetMatrixWorld();

			GameWorld* world = mOwner->GetWorld();
			mMesh->Draw( world->mRenderer, world );
		}
	}

	void MeshComponent::Shutdown()
	{
		DrawableComponent::Shutdown();
	}

	///////////////////////////////////

	void MeshComponent::Save( Archive& archive )
	{
		_ASSERT( mOwner );
		_ASSERT( mOwner->GetWorld() );
		_ASSERT( mOwner->GetWorld()->mMeshManager );

		SERIAL_REGISTER.Save( archive );

		GameComponent::Save( archive );

		archive.Write( &mOwner->GetWorld()->mMeshManager->Get( mMesh ));
		archive.Write( mMaterial.Ptr(), ar_sizeof( mMaterial ));
	}

	void MeshComponent::Load( Archive& archive )
	{
		_ASSERT( mOwner );
		_ASSERT( mOwner->GetWorld() );
		_ASSERT( mOwner->GetWorld()->mMeshManager );

		GameComponent::Load( archive );

		std::string name;
		archive.Read( &name );

		mMesh = mOwner->GetWorld()->mMeshManager->Get( name );

		archive.Read( mMaterial.Ptr(), ar_sizeof( mMaterial ));
	}
}
