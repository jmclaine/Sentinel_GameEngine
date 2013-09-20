#include "GameObject.h"
#include "DrawableComponent.h"

namespace Sentinel
{
	DrawableComponent::DrawableComponent()
	{
		mComponentType = COMPONENT_DRAWABLE;

		mTransform = NULL;
	}

	void DrawableComponent::Startup()
	{
		mTransform = (TransformComponent*)mOwner->FindComponent( COMPONENT_TRANSFORM );

		if( mTransform == NULL )
			throw AppException( "DrawableComponent::Startup()\n" + std::string( mOwner->mName ) + " does not contain TransformComponent" );
	}

	void DrawableComponent::Update()
	{}

	void DrawableComponent::Shutdown()
	{}
}
