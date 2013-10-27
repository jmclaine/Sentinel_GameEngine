#include "Sprite.h"
#include "Texture.h"
#include "Vector2f.h"

namespace Sentinel
{
	Sprite::Sprite( std::shared_ptr< Texture > texture, std::shared_ptr< Shader > shader, const Point2i& spriteSize )
	{
		_ASSERT( texture );
		_ASSERT( shader );
		_ASSERT( spriteSize.x > 0 && spriteSize.y > 0 );

		mTexture		= texture;
		mShader			= shader;

		mSize			= spriteSize;

		mDimension.x	= texture->Width()  / mSize.x;
		mDimension.y	= texture->Height() / mSize.y;

		mNumFrames		= mDimension.x * mDimension.y;
	}

	Sprite::~Sprite()
	{}

	const Point2i& Sprite::GetSize()
	{
		return mSize;
	}

	const Point2i& Sprite::GetDimension()
	{
		return mDimension;
	}

	void Sprite::SetFrame( UINT frame )
	{
		_ASSERT( frame < mNumFrames );

		mFrame = frame;
	}

	UINT Sprite::GetFrame()
	{
		return mFrame;
	}

	Vector2f Sprite::GetFrameCoords()
	{
		return GetFrameCoords( mFrame );
	}

	Vector2f Sprite::GetFrameCoords( UINT frame )
	{
		_ASSERT( frame < mNumFrames );

		return Vector2f( (float)(frame % mDimension.x), (float)(frame / mDimension.x) );
	}

	UINT Sprite::NumFrames()
	{
		return mNumFrames;
	}

	std::shared_ptr< Texture > Sprite::GetTexture()
	{
		return mTexture;
	}

	std::shared_ptr< Shader > Sprite::GetShader()
	{
		return mShader;
	}
}
