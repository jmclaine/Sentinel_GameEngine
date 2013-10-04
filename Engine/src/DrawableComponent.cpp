#include "GameObject.h"
#include "DrawableComponent.h"

namespace Sentinel
{
	DrawableComponent::DrawableComponent()
	{
		mType = GameComponent::DRAWABLE;

		Shutdown();
	}

	void DrawableComponent::Startup()
	{
		mTransform = (TransformComponent*)mOwner->FindComponent( GameComponent::TRANSFORM );

		if( mTransform == NULL )
			throw AppException( "DrawableComponent::Startup()\n" + std::string( mOwner->mName ) + " does not contain TransformComponent" );
	}

	void DrawableComponent::Update()
	{}

	void DrawableComponent::Shutdown()
	{
		mTransform = NULL;
	}
}
