#include "Texture.h"

namespace Sentinel
{
	Texture::Texture( UINT width, UINT height, ImageFormatType format ) :
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

	const ImageFormatType& Texture::Format()
	{
		return mFormat;
	}
}
