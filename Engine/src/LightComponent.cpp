#include "GameObject.h"
#include "LightComponent.h"

namespace Sentinel
{
	LightComponent::LightComponent()
	{
		mType = GameComponent::LIGHT;

		mTransform = NULL;

		mAttenuation = Vector4f( 1, 1, 1, 10000 );
	}

	const TransformComponent* LightComponent::GetTransform()
	{
		return mTransform;
	}

	void LightComponent::Startup()
	{
		mTransform = (TransformComponent*)mOwner->FindComponent( GameComponent::TRANSFORM );

		if( mTransform == NULL )
			throw AppException( "LightComponent::Startup()\n" + std::string( mOwner->mName ) + " does not contain TransformComponent" );
	}

	void LightComponent::Update()
	{}

	void LightComponent::Shutdown()
	{}
}