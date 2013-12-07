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

		mModel->GetMaterials( &mMaterial );
	}

	void ModelComponent::Startup()
	{
		DrawableComponent::Startup();
	}

	void ModelComponent::Update()
	{
		DrawableComponent::Update();

		if( mTransform )
		{
			mModel->SetMaterials( mMaterial );

			mModel->mMatrixWorld = mTransform->GetMatrixWorld();

			GameWorld* world = mOwner->GetWorld();
			mModel->Draw( world->mRenderer, world );
		}
	}

	void ModelComponent::Shutdown()
	{
		DrawableComponent::Shutdown();
	}

	///////////////////////////////////

	void ModelComponent::SetMaterial( const Material& material )
	{
		for( UINT x = 0; x < mMaterial.size(); ++x )
			mMaterial[ x ] = material;
	}

	void ModelComponent::SetMaterial( UINT index, const Material& material )
	{
		_ASSERT( index < mMaterial.size() );

		mMaterial[ index ] = material;
	}

	void ModelComponent::SetMaterial( const std::vector< Material >& material )
	{
		mMaterial = material;
	}

	const std::vector< Material >& ModelComponent::GetMaterial()
	{
		return mMaterial;
	}

	UINT ModelComponent::GetMaterialSize()
	{
		return mMaterial.size();
	}

	///////////////////////////////////

	void ModelComponent::Save( Archive& archive )
	{
		_ASSERT( mOwner );
		_ASSERT( mOwner->GetWorld() );
		_ASSERT( mOwner->GetWorld()->mModelManager );

		SERIAL_REGISTER.Save( archive );

		GameComponent::Save( archive );

		archive.Write( &mOwner->GetWorld()->mModelManager->Get( mModel ));

		UINT count = (UINT)mMaterial.size();
		archive.Write( &count );

		for( UINT x = 0; x < count; ++x )
			archive.Write( mMaterial[ x ].Ptr(), ar_sizeof( mMaterial[ x ] ));
	}

	void ModelComponent::Load( Archive& archive )
	{
		_ASSERT( mOwner );
		_ASSERT( mOwner->GetWorld() );
		_ASSERT( mOwner->GetWorld()->mModelManager );

		GameComponent::Load( archive );

		std::string name;
		archive.Read( &name );

		mModel = mOwner->GetWorld()->mModelManager->Get( name );

		UINT count;
		archive.Read( &count );

		mMaterial.resize( count );

		for( UINT x = 0; x < count; ++x )
			archive.Read( mMaterial[ x ].Ptr(), ar_sizeof( mMaterial[ x ] ));
	}
}
