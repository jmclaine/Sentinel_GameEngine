#include "SpriteComponent.h"
#include "Vector2f.h"
#include "Matrix4f.h"
#include "Material.h"
#include "Sprite.h"
#include "GameObject.h"
#include "GameWorld.h"
#include "Mesh.h"
#include "TransformComponent.h"
#include "CameraComponent.h"

namespace Sentinel
{
	DEFINE_SERIAL_REGISTER( SpriteComponent );
	DEFINE_SERIAL_CLONE( SpriteComponent );

	SpriteComponent::SpriteComponent() :
		mCamera( NULL ),
		mSpriteSystem( NULL ),
		mFrame( 0 )
	{}

	SpriteComponent::SpriteComponent( SpriteSystem* spriteSystem, CameraComponent* camera ) :
		mFrame( 0 )
	{
		Set( spriteSystem, camera );
	}

	SpriteComponent::~SpriteComponent()
	{
		delete mSpriteSystem;
	}

	void SpriteComponent::Set( SpriteSystem* spriteSystem, CameraComponent* camera )
	{
		mSpriteSystem = spriteSystem;
		mCamera = camera;
	}

	/////////////////////////////////

	void SpriteComponent::Startup()
	{
		DrawableComponent::Startup();

		mTransform = (TransformComponent*)mOwner->FindComponent( GameComponent::TRANSFORM );

		if( !mTransform )
			throw AppException( "SpriteComponent::Startup()\n" + std::string( mOwner->mName ) + " does not contain TransformComponent" );

		if( !mSpriteSystem )
			throw AppException( "SpriteComponent::Startup()\n" + std::string( mOwner->mName ) + " does not contain SpriteSystem" );

		if( !mCamera )
			throw AppException( "SpriteComponent::Startup()\n" + std::string( mOwner->mName ) + " does not contain CameraComponent" );
	}

	void SpriteComponent::Update()
	{
		DrawableComponent::Update();

		mSpriteSystem->Clear();

		mSpriteSystem->Draw( mFrame, mColor, mCamera->mMatrixFinal * mTransform->GetMatrixWorld() );

		mSpriteSystem->Present();
	}

	void SpriteComponent::Shutdown()
	{
		DrawableComponent::Shutdown();
	}

	/////////////////////////////////

	void SpriteComponent::Save( Archive& archive )
	{
		mSerialRegistry.Save( archive );

		GameComponent::Save( archive );
	}

	void SpriteComponent::Load( Archive& archive )
	{
		GameComponent::Load( archive );
	}
}
