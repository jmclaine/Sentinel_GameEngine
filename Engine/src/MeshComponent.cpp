#include <vector>

#include "MeshComponent.h"
#include "MeshManager.h"
#include "Mesh.h"
#include "TransformComponent.h"
#include "GameObject.h"
#include "GameWorld.h"
#include "Archive.h"
#include "Buffer.h"

namespace Sentinel
{
	DEFINE_SERIAL_REGISTER( MeshComponent );
	DEFINE_SERIAL_CLONE( MeshComponent );

	MeshComponent::MeshComponent()
	{}

	MeshComponent::MeshComponent( std::shared_ptr< Mesh > mesh )
	{
		mMesh = mesh;
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

		const BoundingBox& bounds = mMesh->mBounds;
		const Matrix4f& matWorld  = mTransform->GetMatrixWorld();

		mBounds.Set( bounds.GetMinBounds(), bounds.GetMaxBounds(), matWorld );

		mMesh->mMatrixWorld = matWorld;

		GameWorld* world = mOwner->GetWorld();

		mMesh->Draw( world->mRenderer, world );
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

		GameWorld* world = mOwner->GetWorld();

		archive.Write( &world->mMeshManager->Get( mMesh ));
	}

	void MeshComponent::Load( Archive& archive )
	{
		_ASSERT( mOwner );
		_ASSERT( mOwner->GetWorld() );
		_ASSERT( mOwner->GetWorld()->mMeshManager );

		GameComponent::Load( archive );

		GameWorld* world = mOwner->GetWorld();

		std::string name;
		archive.Read( &name );

		mMesh = world->mMeshManager->Get( name );
	}
}
