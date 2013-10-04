#include "TransformComponent.h"
#include "GameObject.h"

namespace Sentinel
{
	Matrix4f TransformComponent::mMatrixTranslation;
	Matrix4f TransformComponent::mMatrixScale;
	Matrix4f TransformComponent::mMatrixRotation;

	DEFINE_SERIAL_REGISTER( TransformComponent );
	DEFINE_SERIAL_CLONE( TransformComponent );

	TransformComponent::TransformComponent()
	{
		mType = GameComponent::TRANSFORM;

		Shutdown();

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
	{
		mParentTransform = NULL;
	}

	///////////////////////////////////

	const Matrix4f& TransformComponent::GetMatrixWorld()
	{
		return mMatrixWorld;
	}

	///////////////////////////////////

	void TransformComponent::Save( Archive& archive )
	{
		mSerialRegistry.Save( archive );

		GameComponent::Save( archive );

		archive.Write( mPosition.Ptr(), 3 );
		archive.Write( mOrientation.Ptr(), 4 );
		archive.Write( mScale.Ptr(), 3 );
	}

	void TransformComponent::Load( Archive& archive )
	{
		GameComponent::Load( archive );

		archive.Read( mPosition.Ptr(), 3 );
		archive.Read( mOrientation.Ptr(), 4 );
		archive.Read( mScale.Ptr(), 3 );
	}
}
