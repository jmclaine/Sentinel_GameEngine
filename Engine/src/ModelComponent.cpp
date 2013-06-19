#include "ModelComponent.h"

namespace Sentinel
{
	ModelComponent::ModelComponent( std::shared_ptr< Model > model, const Material& material )
	{
		mModel = model;
		mMaterial = material;
	}

	void ModelComponent::Startup()
	{
		DrawableComponent::Startup();
	}

	void ModelComponent::Update()
	{
		DrawableComponent::Update();

		mModel->SetMaterial( mMaterial );

		mModel->mMatrixWorld = mMatrixTranslation * mMatrixScale * mMatrixRotation;

		mModel->Draw();
	}

	void ModelComponent::Shutdown()
	{
		mModel.reset();
	}
}
