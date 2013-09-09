#include "ModelComponent.h"

namespace Sentinel
{
	ModelComponent::ModelComponent( std::shared_ptr< Model > model )
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

	/////////////////////////////

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
}
