#include "GameObject.h"
#include "LightComponent.h"

namespace Sentinel
{
	LightComponent::LightComponent()
	{
		mComponentType = COMPONENT_LIGHT;

		mTransform = NULL;
	}

	const TransformComponent* LightComponent::GetTransform()
	{
		return mTransform;
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