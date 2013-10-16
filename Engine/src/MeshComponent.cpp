#include <vector>

#include "MeshComponent.h"
#include "MeshManager.h"
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
		_ASSERT( mOwner->World() );
		_ASSERT( mOwner->World()->mRenderer );

		DrawableComponent::Startup();
	}

	void MeshComponent::Update()
	{
		DrawableComponent::Update();

		if( mTransform )
		{
			mMesh->mMaterial = mMaterial;

			mMesh->mMatrixWorld = mTransform->GetMatrixWorld();

			GameWorld* world = mOwner->World();
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
		_ASSERT( mOwner->World() );
		_ASSERT( mOwner->World()->mMeshManager );

		mSerialRegistry.Save( archive );

		GameComponent::Save( archive );

		archive.Write( &mOwner->World()->mMeshManager->Get( mMesh ));
		archive.Write( mMaterial.Ptr(), ar_sizeof( mMaterial ));
	}

	void MeshComponent::Load( Archive& archive )
	{
		_ASSERT( mOwner );
		_ASSERT( mOwner->World() );
		_ASSERT( mOwner->World()->mMeshManager );

		GameComponent::Load( archive );

		std::string name;
		archive.Read( &name );

		mMesh = mOwner->World()->mMeshManager->Get( name );

		archive.Read( mMaterial.Ptr(), ar_sizeof( mMaterial ));
	}
}
