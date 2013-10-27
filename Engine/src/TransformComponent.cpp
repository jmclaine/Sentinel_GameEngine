#include "TransformComponent.h"
#include "GameObject.h"
#include "Archive.h"

namespace Sentinel
{
	Matrix4f TransformComponent::MATRIX_TRANSLATION;
	Matrix4f TransformComponent::MATRIX_ROTATION;
	Matrix4f TransformComponent::MATRIX_SCALE;

	DEFINE_SERIAL_REGISTER( TransformComponent );
	DEFINE_SERIAL_CLONE( TransformComponent );

	TransformComponent::TransformComponent() :
		mParentTransform( NULL ),
		mScale( Vector3f( 1, 1, 1 ))
	{
		mType = GameComponent::TRANSFORM;
	}

	void TransformComponent::Startup()
	{
		if( mOwner->Parent() )
			mParentTransform = static_cast< TransformComponent* >(mOwner->Parent()->FindComponent( GameComponent::TRANSFORM ));
		else
			mParentTransform = NULL;
	}

	void TransformComponent::Update()
	{
		MATRIX_TRANSLATION.Translate( mPosition );
		MATRIX_ROTATION.Rotate( mOrientation );
		MATRIX_SCALE.Scale( mScale );

		mMatrixWorld = MATRIX_TRANSLATION * MATRIX_ROTATION * MATRIX_SCALE;

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

		archive.Write( mPosition.Ptr(),		ar_sizeof( mPosition ));
		archive.Write( mOrientation.Ptr(),  ar_sizeof( mOrientation ));
		archive.Write( mScale.Ptr(),		ar_sizeof( mScale ));
	}

	void TransformComponent::Load( Archive& archive )
	{
		GameComponent::Load( archive );

		archive.Read( mPosition.Ptr(),		ar_sizeof( mPosition ));
		archive.Read( mOrientation.Ptr(),	ar_sizeof( mOrientation ));
		archive.Read( mScale.Ptr(),			ar_sizeof( mScale ));
	}
}
