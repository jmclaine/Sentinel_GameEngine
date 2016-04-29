#include "Texture.h"

namespace Sentinel
{
	Texture::Texture( UINT width, UINT height, ImageFormat format ) :
		mWidth( width ),
		mHeight( height ),
		mFormat( format )
	{ }

	Texture::~Texture()
	{ }

	UINT Texture::Width()
	{
		return mWidth;
	}

	UINT Texture::Height()
	{
		return mHeight;
	}

	const ImageFormat& Texture::Format()
	{
		return mFormat;
	}
}
