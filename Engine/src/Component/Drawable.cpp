#include "GameObject.h"
#include "GameWorld.h"
#include "Component/Camera.h"
#include "Component/Drawable.h"
#include "Shape.h"
#include "Archive.h"

namespace Sentinel { namespace Component
{
	Drawable::Drawable() :
		mTransform( NULL ),
		mIsDynamic( false )
	{
		mType = GameComponent::DRAWABLE;
	}

	void Drawable::Startup()
	{
		mTransform = (Transform*)mOwner->FindComponent( GameComponent::TRANSFORM );

		if( mTransform == NULL )
			throw AppException( "Drawable::Startup()\n" + std::string( mOwner->mName ) + " does not contain Transform" );
	}

	void Drawable::Update()
	{
		CalculateBounds();
	}

	void Drawable::Shutdown()
	{
		mTransform = NULL;
	}

	///////////////////////////////////

	void Drawable::Save( Archive& archive )
	{
		GameComponent::Save( archive );

		archive.Write( &mIsDynamic );
	}

	void Drawable::Load( Archive& archive )
	{
		GameComponent::Load( archive );

		archive.Read( &mIsDynamic );
	}

	void Drawable::Copy( GameComponent* component )
	{
		GameComponent::Copy( component );

		((Drawable*)component)->mIsDynamic = mIsDynamic;
	}
}}
