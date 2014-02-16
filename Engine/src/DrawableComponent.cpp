#include "GameObject.h"
#include "DrawableComponent.h"
#include "Archive.h"

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

	///////////////////////////////////

	void DrawableComponent::Save( Archive& archive )
	{
		GameComponent::Save( archive );

		archive.Write( &mIsDynamic );
	}

	void DrawableComponent::Load( Archive& archive )
	{
		GameComponent::Load( archive );

		archive.Read( &mIsDynamic );
	}
}
