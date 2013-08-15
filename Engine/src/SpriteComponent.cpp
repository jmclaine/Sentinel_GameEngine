#include "SpriteComponent.h"

namespace Sentinel
{
	SpriteComponent::SpriteComponent( Texture* texture, const POINT& spriteSize )
	{
		_ASSERT( mTexture );
		_ASSERT( spriteSize.x > 0 && spriteSize.y > 0 );

		mComponentType	= COMPONENT_DRAWABLE;

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

		ParticleSystem::Inst()->Begin( this );

		UCHAR* verts = (UCHAR*)ParticleSystem::Inst()->mVertex;

		if( mTexture )
		{
			ParticleSystem::Inst()->mMesh->SetTexture( mTexture );

			*(Vector2f*)verts = Vector2f( (float)(mFrame % mSpriteDimension.x), \
										  (float)(mFrame / mSpriteDimension.x) );
		}
		verts += sizeof( Vector2f );

		*(UINT*)verts = mColor.ToUINT();
		verts += sizeof( UINT );

		*(Matrix4f*)verts = mMatrixTranslation * mMatrixRotation * mMatrixScale;
		verts += sizeof( Matrix4f );

		ParticleSystem::Inst()->End();
	}

	void SpriteComponent::Shutdown()
	{}
}
