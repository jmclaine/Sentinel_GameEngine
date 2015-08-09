#include "Component/Transform.h"
#include "GameObject.h"
#include "Archive.h"

namespace Sentinel { namespace Component
{
	DEFINE_SERIAL_REGISTER( Transform );
	DEFINE_SERIAL_CLONE( Transform );

	Transform::Transform() :
		mParentTransform( NULL ),
		mScale( Vector3f( 1, 1, 1 ))
	{
		mType = GameComponent::TRANSFORM;
	}

	void Transform::Startup()
	{
		if( mOwner->GetParent() )
			mParentTransform = static_cast< Transform* >(mOwner->GetParent()->FindComponent( GameComponent::TRANSFORM ));
		else
			mParentTransform = NULL;
	}

	void Transform::Update()
	{
		mMatrixWorld.World( mPosition, mOrientation, mScale );
		
		if( mParentTransform )
			mMatrixWorld = mParentTransform->GetMatrixWorld() * mMatrixWorld;
	}

	void Transform::Shutdown()
	{
		mParentTransform = NULL;
	}

	///////////////////////////////////

	const Matrix4f& Transform::GetMatrixWorld() const
	{
		return mMatrixWorld;
	}

	const Matrix4f& Transform::GetMatrixWorld( const Vector3f& offset )
	{
		mMatrixWorld.World( mPosition, mOrientation, mScale, offset );

		if( mParentTransform )
			mMatrixWorld = mParentTransform->GetMatrixWorld() * mMatrixWorld;

		return mMatrixWorld;
	}

	///////////////////////////////////

	void Transform::Save( Archive& archive )
	{
		SERIAL_REGISTER.Save( archive );

		GameComponent::Save( archive );

		archive.Write( mPosition.Ptr(),		ar_sizeof( mPosition ));
		archive.Write( mOrientation.Ptr(),  ar_sizeof( mOrientation ));
		archive.Write( mScale.Ptr(),		ar_sizeof( mScale ));
	}

	void Transform::Load( Archive& archive )
	{
		GameComponent::Load( archive );

		archive.Read( mPosition.Ptr(),		ar_sizeof( mPosition ));
		archive.Read( mOrientation.Ptr(),	ar_sizeof( mOrientation ));
		archive.Read( mScale.Ptr(),			ar_sizeof( mScale ));
	}

	///////////////////////////////////

	GameComponent* Transform::Copy()
	{
		Transform* transform = new Transform();

		GameComponent::Copy( transform );

		transform->mParentTransform = mParentTransform;
		transform->mPosition = mPosition;
		transform->mOrientation = mOrientation;
		transform->mScale = mScale;

		return transform;
	}
}}
