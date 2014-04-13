#include "Texture.h"

namespace Sentinel
{
	Texture::Texture( UINT width, UINT height, ImageFormat::Type format ) :
		mWidth( width ),
		mHeight( height ),
		mFormat( format )
	{}

	Texture::~Texture()
	{}

	UINT Texture::Width()
	{
		return mWidth;
	}

	UINT Texture::Height()
	{
		return mHeight;
	}

	const ImageFormat::Type& Texture::Format()
	{
		return mFormat;
	}
}
