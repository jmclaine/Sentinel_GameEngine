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

	WTransformComponent::WTransformComponent( WGameComponent^ component )
	{
		mRef = component->GetRef();
	}

	WTransformComponent::~WTransformComponent()
	{}

	/////////////////////////////////

	DEFINE_PROPERTY_R( TransformComponent, Vector3f, Position );
	DEFINE_PROPERTY_R( TransformComponent, Quatf,	 Orientation );
	DEFINE_PROPERTY_R( TransformComponent, Vector3f, Scale );

	/////////////////////////////////

	WMatrix4f^ WTransformComponent::GetMatrixWorld()
	{
		return gcnew WMatrix4f( static_cast< TransformComponent* >(mRef)->GetMatrixWorld() );
	}
}}
