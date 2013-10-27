#include "SpriteComponent.h"
#include "ParticleSystem.h"
#include "Vector2f.h"
#include "Matrix4f.h"
#include "Material.h"
#include "Sprite.h"
#include "GameObject.h"
#include "Mesh.h"
#include "TransformComponent.h"

namespace Sentinel
{
	DEFINE_SERIAL_REGISTER( SpriteComponent );
	DEFINE_SERIAL_CLONE( SpriteComponent );

	SpriteComponent::SpriteComponent() :
		mParticle( NULL ), mSprite( NULL )
	{}

	SpriteComponent::SpriteComponent( ParticleSystem* particle, std::shared_ptr< Sprite > sprite )
	{
		Set( particle, sprite );
	}

	void SpriteComponent::Set( ParticleSystem* particle, std::shared_ptr< Sprite > sprite )
	{
		mParticle = particle;
		mSprite   = sprite;
	}

	/////////////////////////////////

	void SpriteComponent::Startup()
	{
		DrawableComponent::Startup();

		if( !mParticle )
			throw AppException( "SpriteComponent::Startup()\n" + std::string( mOwner->mName ) + " does not contain ParticleSystem" );

		if( !mTransform )
			throw AppException( "SpriteComponent::Startup()\n" + std::string( mOwner->mName ) + " does not contain TransformComponent" );

		if( !mSprite )
			throw AppException( "SpriteComponent::Startup()\n" + std::string( mOwner->mName ) + " does not contain Sprite" );
	}

	void SpriteComponent::Update()
	{
		DrawableComponent::Update();

		if( mTransform )
		{}
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
