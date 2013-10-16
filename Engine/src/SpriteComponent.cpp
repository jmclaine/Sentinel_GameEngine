#include "SpriteComponent.h"
#include "ParticleSystem.h"
#include "Vector2f.h"
#include "Matrix4f.h"
#include "Material.h"
#include "Texture.h"
#include "GameObject.h"
#include "Mesh.h"
#include "TransformComponent.h"

namespace Sentinel
{
	DEFINE_SERIAL_REGISTER( SpriteComponent );
	DEFINE_SERIAL_CLONE( SpriteComponent );

	SpriteComponent::SpriteComponent() :
		mParticle( NULL ), mTexture( NULL )
	{}

	SpriteComponent::SpriteComponent( ParticleSystem* particle, std::shared_ptr< Texture >texture, const POINT& spriteSize )
	{
		Set( particle, texture, spriteSize );
	}

	void SpriteComponent::Set( ParticleSystem* particle, std::shared_ptr< Texture > texture, const POINT& spriteSize )
	{
		_ASSERT( mTexture );
		_ASSERT( spriteSize.x > 0 && spriteSize.y > 0 );

		mTexture			= texture;
		mSpriteSize			= spriteSize;

		mSpriteDimension.x	= texture->Width()  / mSpriteSize.x;
		mSpriteDimension.y	= texture->Height() / mSpriteSize.y;

		mNumFrames			= mSpriteDimension.x * mSpriteDimension.y;
	}

	void SpriteComponent::Startup()
	{
		DrawableComponent::Startup();

		if( !mTexture )
			throw AppException( "SpriteComponent::Startup()\n" + std::string( mOwner->mName ) + " does not contain Texture" );

		if( !mParticle )
			throw AppException( "SpriteComponent::Startup()\n" + std::string( mOwner->mName ) + " does not contain ParticleSystem" );
	}

	void SpriteComponent::Update()
	{
		DrawableComponent::Update();

		if( mTransform )
		{
			mParticle->Begin( this );

			UCHAR* verts = (UCHAR*)mParticle->mVertex;

			if( mTexture )
			{
				mParticle->mMesh->mTexture[ TEXTURE_DIFFUSE ] = mTexture;

				*(Vector2f*)verts = Vector2f( (float)(mFrame % mSpriteDimension.x), \
											  (float)(mFrame / mSpriteDimension.x) );
			}
			verts += sizeof( Vector2f );

			*(UINT*)verts = mColor.ToUINT();
			verts += sizeof( UINT );

			*(Matrix4f*)verts = mTransform->GetMatrixWorld();
			verts += sizeof( Matrix4f );

			mParticle->End();
		}
	}

	void SpriteComponent::Shutdown()
	{
		DrawableComponent::Shutdown();
	}

	/////////////////////////////////

	void SpriteComponent::SetMaterial( const Material& material )
	{
		mParticle->mMesh->mMaterial = material;
	}

	const Material& SpriteComponent::GetMaterial()
	{
		return mParticle->mMesh->mMaterial;
	}

	void SpriteComponent::SetColor( const ColorRGBA& color )
	{
		mColor = color;
	}

	void SpriteComponent::SetFrame( UINT frame )
	{
		mFrame = frame;
	}

	///////////////////////////////////

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
