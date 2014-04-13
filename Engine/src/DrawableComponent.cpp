#include "GameObject.h"
#include "GameWorld.h"
#include "CameraComponent.h"
#include "Shape.h"
#include "DrawableComponent.h"
#include "Archive.h"

namespace Sentinel
{
	DrawableComponent::DrawableComponent() :
		mTransform( NULL ),
		mIsDynamic( false ),
		mIsVisible( true )
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
	{
		CalculateBounds();

		mIsVisible = mOwner->GetWorld()->GetCamera()->GetFrustum().Intersects( mBounds );
	}

	void DrawableComponent::Shutdown()
	{
		mTransform = NULL;
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
