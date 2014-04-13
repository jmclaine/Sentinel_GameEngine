#include <vector>

#include "MeshComponent.h"
#include "MeshManager.h"
#include "Mesh.h"
#include "TransformComponent.h"
#include "GameObject.h"
#include "GameWorld.h"
#include "Archive.h"
#include "RenderManager.h"

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
		
		DrawableComponent::Startup();

		if( mOwner->GetWorld() )
			mOwner->GetWorld()->mRenderManager->Draw( mMesh->mMaterial->mRenderQueue, this );
	}

	void MeshComponent::Update()
	{
		DrawableComponent::Update();
	}

	void MeshComponent::Shutdown()
	{
		DrawableComponent::Shutdown();
	}

	///////////////////////////////////

	void MeshComponent::CalculateBounds()
	{
		const BoundingBox& bounds = mMesh->mBounds;
		
		mBounds.Set( bounds.GetMinBounds(), bounds.GetMaxBounds(), mTransform->GetMatrixWorld() );
	}

	void MeshComponent::Draw()
	{
		_ASSERT( mOwner->GetWorld() );

		mMesh->mMatrixWorld = mTransform->GetMatrixWorld();

		GameWorld* world = mOwner->GetWorld();

		mMesh->Draw( world->mRenderer, world );
	}

	///////////////////////////////////

	void MeshComponent::Save( Archive& archive )
	{
		_ASSERT( mOwner );
		_ASSERT( mOwner->GetWorld() );
		_ASSERT( mOwner->GetWorld()->mMeshManager );

		SERIAL_REGISTER.Save( archive );

		DrawableComponent::Save( archive );

		GameWorld* world = mOwner->GetWorld();

		archive.Write( &world->mMeshManager->Get( mMesh ));
	}

	void MeshComponent::Load( Archive& archive )
	{
		_ASSERT( mOwner );
		_ASSERT( mOwner->GetWorld() );
		_ASSERT( mOwner->GetWorld()->mMeshManager );

		DrawableComponent::Load( archive );

		GameWorld* world = mOwner->GetWorld();

		std::string name;
		archive.Read( &name );

		mMesh = world->mMeshManager->Get( name );
	}
}
