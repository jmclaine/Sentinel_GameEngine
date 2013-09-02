#include "ModelComponent.h"

namespace Sentinel
{
	ModelComponent::ModelComponent( std::shared_ptr< Model > model )
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

		mModel->SetMaterial( mMaterial );

		mModel->mMatrixWorld = mTransform->GetMatrixWorld();

		mModel->Draw();
	}

	void ModelComponent::Shutdown()
	{
		mModel.reset();
	}
}
