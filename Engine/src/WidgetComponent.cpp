#include "WidgetComponent.h"
#include "SpriteManager.h"
#include "SpriteSystem.h"
#include "FontSystem.h"
#include "Vector2f.h"
#include "Matrix4f.h"
#include "Sprite.h"
#include "Material.h"
#include "GameObject.h"
#include "GameWorld.h"
#include "Mesh.h"
#include "TransformComponent.h"
#include "CameraComponent.h"
#include "Archive.h"
#include "Renderer.h"
#include "RenderManager.h"

namespace Sentinel
{
	DEFINE_SERIAL_REGISTER( WidgetComponent );
	DEFINE_SERIAL_CLONE( WidgetComponent );

	WidgetComponent::WidgetComponent() :
		mSpriteSystem( NULL ),
		mMaterial( NULL ),
		mFontSystem( NULL ),
		mCamera( -1 )
	{
		mWidgetWorld = new GUI::WidgetWorld();
	}

	WidgetComponent::WidgetComponent( std::shared_ptr< Sprite > sprite, std::shared_ptr< Material > material, FontSystem* fontSystem, UINT camera ) :
		mSpriteSystem( NULL )
	{
		mWidgetWorld = new GUI::WidgetWorld();

		Set( sprite, material, fontSystem, camera );
	}

	WidgetComponent::~WidgetComponent()
	{
		delete mWidgetWorld;
	}

	void WidgetComponent::Set( std::shared_ptr< Sprite > sprite, std::shared_ptr< Material > material, FontSystem* fontSystem, UINT camera )
	{
		_ASSERT( sprite );
		
		mSprite			= sprite;
		mMaterial		= material;
		mFontSystem		= fontSystem;
		mCamera			= camera;
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

		CameraComponent* camera = mOwner->GetWorld()->GetCamera( mCamera );

		if( !camera )
			throw AppException( "WidgetComponent::Startup()\n" + std::string( mOwner->mName ) + " does not contain CameraComponent" );

		mWidgetWorld->mMatrixWVP	= camera->GetMatrixFinal() * mTransform->GetMatrixWorld();
		mWidgetWorld->mWindowInfo	= mOwner->GetWorld()->mRenderer->GetWindow();
		mWidgetWorld->mGameWorld	= mOwner->GetWorld();

		mWidgetWorld->Startup();

		mOwner->GetWorld()->mRenderManager->Draw( RenderQueue::FOREGROUND, this );
	}

	void WidgetComponent::Update()
	{
		//DrawableComponent::Update();
	}

	void WidgetComponent::Shutdown()
	{
		DrawableComponent::Shutdown();

		mWidgetWorld->Shutdown();
	}

	/////////////////////////////////

	void WidgetComponent::CalculateBounds()
	{}

	void WidgetComponent::Draw()
	{
		CameraComponent* camera = mOwner->GetWorld()->GetCamera( mCamera );

		if( camera )
		{
			mWidgetWorld->mMatrixWVP = camera->GetMatrixFinal() * mTransform->GetMatrixWorld();
			
			if( mSpriteSystem )
			{
				mSpriteSystem->mSprite   = mSprite;
				mSpriteSystem->mMaterial = mMaterial;

				mSpriteSystem->Clear();

				mWidgetWorld->mSpriteSystem = mSpriteSystem;
			}

			if( mFontSystem )
			{
				mWidgetWorld->mFontSystem = mFontSystem;

				mFontSystem->mSpriteSystem->mSprite   = mFontSystem->mFont->mSprite;
				mFontSystem->mSpriteSystem->mMaterial = mFontSystem->mFont->mMaterial;

				mFontSystem->mSpriteSystem->Clear();
			}
			
			mWidgetWorld->Update();

			mSpriteSystem->Present();
			mFontSystem->mSpriteSystem->Present();
		}
	}

	/////////////////////////////////

	void WidgetComponent::Save( Archive& archive )
	{
		_ASSERT( mOwner );
		_ASSERT( mOwner->GetWorld() );
		_ASSERT( mOwner->GetWorld()->mSpriteManager );

		SERIAL_REGISTER.Save( archive );

		DrawableComponent::Save( archive );

		archive.Write( &mCamera );
		archive.Write( &mOwner->GetWorld()->mSpriteManager->Get( mSprite ));

		mWidgetWorld->Save( archive );
	}

	void WidgetComponent::Load( Archive& archive )
	{
		_ASSERT( mOwner );
		_ASSERT( mOwner->GetWorld() );
		_ASSERT( mOwner->GetWorld()->mSpriteManager );

		DrawableComponent::Load( archive );

		archive.Read( &mCamera );

		std::string name;
		archive.Read( &name );

		mSprite = mOwner->GetWorld()->mSpriteManager->Get( name );

		mWidgetWorld->Load( archive );
	}
}
