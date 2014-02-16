#include "ModelComponent.h"
#include "Model.h"
#include "ModelManager.h"
#include "TransformComponent.h"
#include "Material.h"
#include "GameObject.h"
#include "GameWorld.h"
#include "Archive.h"

namespace Sentinel
{
	DEFINE_SERIAL_REGISTER( ModelComponent );
	DEFINE_SERIAL_CLONE( ModelComponent );

	ModelComponent::ModelComponent() :
		mModel( NULL )
	{}

	ModelComponent::ModelComponent( std::shared_ptr< Model > model )
	{
		Set( model );
	}

	void ModelComponent::Set( std::shared_ptr< Model > model )
	{
		mModel = model;
	}

	void ModelComponent::Startup()
	{
		DrawableComponent::Startup();
	}

	void ModelComponent::Update()
	{
		DrawableComponent::Update();

		mModel->mMatrixWorld = mTransform->GetMatrixWorld();

		GameWorld* world = mOwner->GetWorld();

		mModel->Draw( world->mRenderer, world );
	}

	void ModelComponent::Shutdown()
	{
		DrawableComponent::Shutdown();
	}

	///////////////////////////////////

	void ModelComponent::Save( Archive& archive )
	{
		_ASSERT( mOwner );
		_ASSERT( mOwner->GetWorld() );
		_ASSERT( mOwner->GetWorld()->mModelManager );

		SERIAL_REGISTER.Save( archive );

		DrawableComponent::Save( archive );

		GameWorld* world = mOwner->GetWorld();

		archive.Write( &world->mModelManager->Get( mModel ));
	}

	void ModelComponent::Load( Archive& archive )
	{
		_ASSERT( mOwner );
		_ASSERT( mOwner->GetWorld() );
		_ASSERT( mOwner->GetWorld()->mModelManager );

		DrawableComponent::Load( archive );

		GameWorld* world = mOwner->GetWorld();

		std::string name;
		archive.Read( &name );

		mModel = world->mModelManager->Get( name );
	}
}
