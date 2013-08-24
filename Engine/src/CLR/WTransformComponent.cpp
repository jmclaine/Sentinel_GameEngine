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

	RVector3f^ WTransformComponent::Position()
	{
		return gcnew RVector3f( &static_cast< TransformComponent* >(mRef)->mPosition );
	}

	RQuatf^ WTransformComponent::Orientation()
	{
		return gcnew RQuatf( &static_cast< TransformComponent* >(mRef)->mOrientation );
	}

	RVector3f^ WTransformComponent::Scale()
	{
		return gcnew RVector3f( &static_cast< TransformComponent* >(mRef)->mScale );
	}
}}
