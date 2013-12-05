#include "WidgetComponent.h"
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
#include "Renderer.h"

namespace Sentinel
{
	DEFINE_SERIAL_REGISTER( WidgetComponent );
	DEFINE_SERIAL_CLONE( WidgetComponent );

	WidgetComponent::WidgetComponent() :
		mSpriteSystem( NULL ),
		mFontSystem( NULL ),
		mCamera( -1 )
	{}

	WidgetComponent::WidgetComponent( std::shared_ptr< Sprite > sprite, FontSystem* fontSystem, UINT camera ) :
		mSpriteSystem( NULL )
	{
		Set( sprite, fontSystem, camera );
	}

	WidgetComponent::~WidgetComponent()
	{}

	void WidgetComponent::Set( std::shared_ptr< Sprite > sprite, FontSystem* fontSystem, UINT camera )
	{
		_ASSERT( sprite );
		
		mSprite = sprite;
		mFontSystem = fontSystem;
		mCamera = camera;
	}

	/////////////////////////////////

	void WidgetComponent::Startup()
	{
		DrawableComponent::Startup();

		mTransform = (TransformComponent*)mOwner->FindComponent( GameComponent::TRANSFORM );

		if( !mTransform )
			throw AppException( "WidgetComponent::Startup()\n" + std::string( mOwner->mName ) + " does not contain TransformComponent" );

		mSpriteSystem = mOwner->GetWorld()->mSpriteSystem;

		if( !mSpriteSystem )
			throw AppException( "WidgetComponent::Startup()\n" + std::string( mOwner->mName ) + " does not contain SpriteSystem" );

		if( !mOwner->GetWorld()->GetCamera( mCamera ))
			throw AppException( "WidgetComponent::Startup()\n" + std::string( mOwner->mName ) + " does not contain CameraComponent" );
	}

	void WidgetComponent::Update()
	{
		DrawableComponent::Update();

		CameraComponent* camera = mOwner->GetWorld()->GetCamera( mCamera );

		if( camera )
		{
			GUI::Widget::WINDOW_INFO	= mOwner->GetWorld()->mRenderer->GetWindow();
			GUI::Widget::GAME_WORLD		= mOwner->GetWorld();
			GUI::Widget::MATRIX_WVP		= camera->GetMatrixFinal() * mTransform->GetMatrixWorld();

			if( mSpriteSystem )
			{
				mSpriteSystem->mSprite = mSprite;

				mSpriteSystem->Clear();

				GUI::Widget::SPRITE_SYSTEM = mSpriteSystem;
			}

			if( mFontSystem )
			{
				GUI::Widget::FONT_SYSTEM = mFontSystem;

				mFontSystem->mSpriteSystem->mSprite = mFontSystem->mFont->mSprite;
				mFontSystem->mSpriteSystem->Clear();
			}
			
			mRoot.Update();

			mSpriteSystem->Present();
			mFontSystem->mSpriteSystem->Present();
		}
	}

	void WidgetComponent::Shutdown()
	{
		DrawableComponent::Shutdown();
	}

	/////////////////////////////////

	void WidgetComponent::Save( Archive& archive )
	{
		_ASSERT( mOwner );
		_ASSERT( mOwner->GetWorld() );
		_ASSERT( mOwner->GetWorld()->mSpriteManager );

		mSerialRegistry.Save( archive );

		GameComponent::Save( archive );

		archive.Write( &mCamera );
		archive.Write( &mOwner->GetWorld()->mSpriteManager->Get( mSprite ));

		mRoot.Save( archive );
	}

	void WidgetComponent::Load( Archive& archive )
	{
		_ASSERT( mOwner );
		_ASSERT( mOwner->GetWorld() );
		_ASSERT( mOwner->GetWorld()->mSpriteManager );

		GameComponent::Load( archive );

		archive.Read( &mCamera );

		std::string name;
		archive.Read( &name );

		mSprite = mOwner->GetWorld()->mSpriteManager->Get( name );

		mRoot.Load( archive );
	}
}
