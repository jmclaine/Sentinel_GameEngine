#include "GameObject.h"
#include "LightComponent.h"

namespace Sentinel
{
	LightComponent::LightComponent()
	{
		mComponentType = COMPONENT_LIGHT;

		mTransform = NULL;
	}

	void LightComponent::Startup()
	{
		mTransform = (TransformComponent*)mOwner->FindComponent( COMPONENT_TRANSFORM );

		if( mTransform == NULL )
			throw AppException( "LightComponent::Startup()\n" + mOwner->mName + " does not contain TransformComponent" );
	}

	void LightComponent::Update()
	{}

	void LightComponent::Shutdown()
	{}
}