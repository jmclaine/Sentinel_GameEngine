#include "WTransformComponent.h"
#include "TransformComponent.h"

namespace Sentinel { namespace Components
{
	WTransformComponent::WTransformComponent()
	{
		mRef = new TransformComponent();
	}

	WTransformComponent::WTransformComponent( WTransformComponent% obj )
	{
		mRef = obj.mRef;
	}

	WTransformComponent::~WTransformComponent()
	{}

	/////////////////////////////////

	WVector3f^ WTransformComponent::Position()
	{
		return gcnew WVector3f( static_cast< TransformComponent* >(mRef)->mPosition );
	}

	WQuatf^ WTransformComponent::Orientation()
	{
		return gcnew WQuatf( static_cast< TransformComponent* >(mRef)->mOrientation );
	}

	WVector3f^ WTransformComponent::Scale()
	{
		return gcnew WVector3f( static_cast< TransformComponent* >(mRef)->mScale );
	}
}}
