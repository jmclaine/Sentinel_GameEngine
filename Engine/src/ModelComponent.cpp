#include "ModelComponent.h"

namespace Sentinel
{
	DEFINE_SERIAL_REGISTER( ModelComponent );
	DEFINE_SERIAL_CLONE( ModelComponent );

	ModelComponent::ModelComponent()
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

		mModel->SetMaterials( mMaterial );

		mModel->mMatrixWorld = mTransform->GetMatrixWorld();

		mModel->Draw();
	}

	void ModelComponent::Shutdown()
	{
		mModel.reset();
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
		mSerialRegistry.Save( archive );
	}

	void ModelComponent::Load( Archive& archive )
	{}
}
