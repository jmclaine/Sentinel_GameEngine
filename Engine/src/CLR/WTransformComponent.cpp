#include "WTransformComponent.h"

namespace Sentinel { namespace Components
{
	WTransformComponent::WTransformComponent()
	{
		mRef = new TransformComponent();
	}

	WTransformComponent::WTransformComponent( const TransformComponent* component )
	{
		mRef = const_cast< TransformComponent* >(component);
	}

	DEFINE_CAST_COMPONENT( TransformComponent );

	/////////////////////////////////

	DEFINE_PROPERTY_RS( TransformComponent, Vector3f, Position );
	DEFINE_PROPERTY_RS( TransformComponent, Quatf,	 Orientation );
	DEFINE_PROPERTY_RS( TransformComponent, Vector3f, Scale );

	/////////////////////////////////

	WMatrix4f^ WTransformComponent::GetMatrixWorld()
	{
		return gcnew WMatrix4f( static_cast< TransformComponent* >(mRef)->GetMatrixWorld() );
	}
}}
