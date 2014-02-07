#include "GameObject.h"
#include "DrawableComponent.h"

namespace Sentinel
{
	DrawableComponent::DrawableComponent() :
		mTransform( NULL ),
		mIsDynamic( false )
	{
		mType = GameComponent::DRAWABLE;
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

	///////////////////////////////////

	const BoundingBox& DrawableComponent::GetBounds()
	{
		return mBounds;
	}
}
