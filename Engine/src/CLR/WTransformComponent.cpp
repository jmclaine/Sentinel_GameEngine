#include "WTransformComponent.h"
#include "MathCommon.h"

namespace Sentinel { namespace Wrapped
{
	WTransformComponent::WTransformComponent()
	{
		mRef = new TransformComponent();

		mRotation = gcnew WVector3f();
	}

	WTransformComponent::WTransformComponent( const TransformComponent* component )
	{
		mRef = const_cast< TransformComponent* >(component);

		mRotation = gcnew WVector3f( static_cast< TransformComponent* >(mRef)->mOrientation.ToEuler() * (float)RADIANS_TO_DEGREES );
	}

	DEFINE_CAST_COMPONENT( TransformComponent );

	/////////////////////////////////

	void WTransformComponent::Update()
	{
		static_cast< TransformComponent* >(mRef)->mOrientation = Quatf( mRotation );

		WGameComponent::Update();
	}

	/////////////////////////////////

	DEFINE_PROPERTY_RS( TransformComponent, Vector3f, Position );
	
	WVector3f^ WTransformComponent::Rotation::get()
	{
		return gcnew RVector3f( mRotation );
	}

	void WTransformComponent::Rotation::set( WVector3f^ v )
	{
		mRotation = v;
		
		static_cast< TransformComponent* >(mRef)->mOrientation = Quatf( mRotation );
	}

	DEFINE_PROPERTY_RS( TransformComponent, Vector3f, Scale );

	/////////////////////////////////

	WMatrix4f^ WTransformComponent::GetMatrixWorld()
	{
		return gcnew WMatrix4f( static_cast< TransformComponent* >(mRef)->GetMatrixWorld() );
	}
}}
