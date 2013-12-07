#include "SpriteComponent.h"
#include "SpriteManager.h"
#include "SpriteSystem.h"
#include "Vector2f.h"
#include "Matrix4f.h"
#include "Material.h"
#include "Sprite.h"
#include "GameObject.h"
#include "GameWorld.h"
#include "Mesh.h"
#include "TransformComponent.h"
#include "CameraComponent.h"
#include "Archive.h"

namespace Sentinel
{
	DEFINE_SERIAL_REGISTER( SpriteComponent );
	DEFINE_SERIAL_CLONE( SpriteComponent );

	SpriteComponent::SpriteComponent() :
		mSpriteSystem( NULL ),
		mFrame( 0 )
	{}

	SpriteComponent::SpriteComponent( std::shared_ptr< Sprite > sprite, UINT camera ) :
		mSpriteSystem( NULL ),
		mFrame( 0 )
	{
		Set( sprite, camera );
	}

	SpriteComponent::~SpriteComponent()
	{}

	void SpriteComponent::Set( std::shared_ptr< Sprite > sprite, UINT camera )
	{
		_ASSERT( sprite );
		_ASSERT( camera != -1 );

		mSprite = sprite;
		mCamera = camera;
	}

	/////////////////////////////////

	void SpriteComponent::Startup()
	{
		DrawableComponent::Startup();

		mTransform = (TransformComponent*)mOwner->FindComponent( GameComponent::TRANSFORM );

		if( !mTransform )
			throw AppException( "SpriteComponent::Startup()\n" + std::string( mOwner->mName ) + " does not contain TransformComponent" );

		mSpriteSystem = mOwner->GetWorld()->mSpriteSystem;

		if( !mSpriteSystem )
			throw AppException( "SpriteComponent::Startup()\n" + std::string( mOwner->mName ) + " does not contain SpriteSystem" );

		if( !mOwner->GetWorld()->GetCamera( mCamera ))
			throw AppException( "WidgetComponent::Startup()\n" + std::string( mOwner->mName ) + " does not contain CameraComponent" );
	}

	void SpriteComponent::Update()
	{
		DrawableComponent::Update();

		mSpriteSystem->mSprite = mSprite;

		mSpriteSystem->Clear();

		mSpriteSystem->Draw( mFrame, mColor, mOwner->GetWorld()->GetCamera( mCamera )->GetMatrixFinal() * mTransform->GetMatrixWorld() );

		mSpriteSystem->Present();
	}

	void SpriteComponent::Shutdown()
	{
		DrawableComponent::Shutdown();
	}

	/////////////////////////////////

	void SpriteComponent::Save( Archive& archive )
	{
		_ASSERT( mOwner );
		_ASSERT( mOwner->GetWorld() );
		_ASSERT( mOwner->GetWorld()->mSpriteManager );

		SERIAL_REGISTER.Save( archive );

		GameComponent::Save( archive );

		archive.Write( &mOwner->GetWorld()->mSpriteManager->Get( mSprite ));
		archive.Write( &mCamera );
		archive.Write( &mFrame );
		archive.Write( mColor.Ptr(), ar_sizeof( mColor ));
	}

	void SpriteComponent::Load( Archive& archive )
	{
		_ASSERT( mOwner );
		_ASSERT( mOwner->GetWorld() );
		_ASSERT( mOwner->GetWorld()->mSpriteManager );

		GameComponent::Load( archive );

		std::string name;
		archive.Read( &name );

		mSprite = mOwner->GetWorld()->mSpriteManager->Get( name );
		
		archive.Read( &mCamera );
		archive.Read( &mFrame );
		archive.Read( mColor.Ptr(), ar_sizeof( mColor ));
	}
}
