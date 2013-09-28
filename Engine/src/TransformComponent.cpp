#include "TransformComponent.h"
#include "GameObject.h"

namespace Sentinel
{
	Matrix4f TransformComponent::mMatrixTranslation;
	Matrix4f TransformComponent::mMatrixScale;
	Matrix4f TransformComponent::mMatrixRotation;

	TransformComponent::TransformComponent() :
		GameComponent()
	{
		mType = GameComponent::TRANSFORM;

		mParentTransform = NULL;

		mScale = Vector3f( 1, 1, 1 );
	}

	void TransformComponent::Startup()
	{
		if( mOwner->mParent )
			mParentTransform = static_cast< TransformComponent* >(mOwner->mParent->FindComponent( GameComponent::TRANSFORM ));
		else
			mParentTransform = NULL;
	}

	void TransformComponent::Update()
	{
		mMatrixTranslation.Translate( mPosition );
		mMatrixRotation.Rotate( mOrientation );
		mMatrixScale.Scale( mScale );

		mMatrixWorld = mMatrixTranslation * mMatrixRotation * mMatrixScale;

		if( mParentTransform )
			mMatrixWorld = mParentTransform->GetMatrixWorld() * mMatrixWorld;
	}

	void TransformComponent::Shutdown()
	{}

	///////////////////////////////////

	const Matrix4f& TransformComponent::GetMatrixWorld()
	{
		return mMatrixWorld;
	}
}
