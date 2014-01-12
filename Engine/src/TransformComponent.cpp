#include "TransformComponent.h"
#include "GameObject.h"
#include "Archive.h"

namespace Sentinel
{
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
		if( mOwner->GetParent() )
			mParentTransform = static_cast< TransformComponent* >(mOwner->GetParent()->FindComponent( GameComponent::TRANSFORM ));
		else
			mParentTransform = NULL;
	}

	void TransformComponent::Update()
	{
		mMatrixWorld.World( mPosition, mOrientation, mScale );
		
		if( mParentTransform )
			mMatrixWorld = mParentTransform->GetMatrixWorld() * mMatrixWorld;
	}

	void TransformComponent::Shutdown()
	{
		mParentTransform = NULL;
	}

	///////////////////////////////////

	const Matrix4f& TransformComponent::GetMatrixWorld() const
	{
		return mMatrixWorld;
	}

	///////////////////////////////////

	void TransformComponent::Save( Archive& archive )
	{
		SERIAL_REGISTER.Save( archive );

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
