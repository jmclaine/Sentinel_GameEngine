#include "GameObject.h"
#include "DrawableComponent.h"

namespace Sentinel
{
	Matrix4f DrawableComponent::mMatrixTranslation;
	Matrix4f DrawableComponent::mMatrixScale;
	Matrix4f DrawableComponent::mMatrixRotation;

	DrawableComponent::DrawableComponent()
	{
		mComponentType = COMPONENT_DRAWABLE;

		mTransform = NULL;
	}

	void DrawableComponent::Startup()
	{
		mTransform = (TransformComponent*)mOwner->FindComponent( COMPONENT_TRANSFORM );

		if( mTransform == NULL )
			throw AppException( "DrawableComponent::Startup()\n" + mOwner->mName + " does not contain TransformComponent" );
	}

	void DrawableComponent::Update()
	{
		mMatrixTranslation.Translate( mTransform->mPosition );

		mMatrixRotation.Rotate( mTransform->mOrientation );

		mMatrixScale.Scale( mTransform->mScale );
	}

	void DrawableComponent::Shutdown()
	{}
}
