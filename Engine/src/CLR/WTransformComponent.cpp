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

	WTransformComponent::~WTransformComponent()
	{}

	/////////////////////////////////

	DEFINE_PROPERTY_R( TransformComponent, Vector3f, Position );
	DEFINE_PROPERTY_R( TransformComponent, Quatf,	 Orientation );
	DEFINE_PROPERTY_R( TransformComponent, Vector3f, Scale );
}}
