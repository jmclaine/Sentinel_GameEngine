#include "SpriteComponent.h"
#include "Vector2f.h"

namespace Sentinel
{
	DEFINE_SERIAL_REGISTER( SpriteComponent );
	DEFINE_SERIAL_CLONE( SpriteComponent );

	SpriteComponent::SpriteComponent()
	{}

	SpriteComponent::SpriteComponent( std::shared_ptr< Texture >texture, const POINT& spriteSize )
	{
		Set( texture, spriteSize );
	}

	void SpriteComponent::Set( std::shared_ptr< Texture > texture, const POINT& spriteSize )
	{
		_ASSERT( mTexture );
		_ASSERT( spriteSize.x > 0 && spriteSize.y > 0 );

		mTexture		= texture;
		mSpriteSize		= spriteSize;

		mSpriteDimension.x = texture->Width()  / mSpriteSize.x;
		mSpriteDimension.y = texture->Height() / mSpriteSize.y;

		mNumFrames = mSpriteDimension.x * mSpriteDimension.y;
	}

	void SpriteComponent::Startup()
	{
		DrawableComponent::Startup();
	}

	void SpriteComponent::Update()
	{
		DrawableComponent::Update();

		if( mTransform )
		{
			ParticleSystem::Inst()->Begin( this );

			UCHAR* verts = (UCHAR*)ParticleSystem::Inst()->mVertex;

			if( mTexture )
			{
				ParticleSystem::Inst()->mMesh->mTexture[ TEXTURE_DIFFUSE ] = mTexture;

				*(Vector2f*)verts = Vector2f( (float)(mFrame % mSpriteDimension.x), \
											  (float)(mFrame / mSpriteDimension.x) );
			}
			verts += sizeof( Vector2f );

			*(UINT*)verts = mColor.ToUINT();
			verts += sizeof( UINT );

			*(Matrix4f*)verts = mTransform->GetMatrixWorld();
			verts += sizeof( Matrix4f );

			ParticleSystem::Inst()->End();
		}
	}

	void SpriteComponent::Shutdown()
	{
		DrawableComponent::Shutdown();
	}

	/////////////////////////////////

	void SpriteComponent::SetMaterial( const Material& material )
	{
		ParticleSystem::Inst()->mMesh->mMaterial = material;
	}

	const Material& SpriteComponent::GetMaterial()
	{
		return ParticleSystem::Inst()->mMesh->mMaterial;
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
