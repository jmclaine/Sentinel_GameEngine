#include "Texture.h"

namespace Sentinel
{
	Texture::Texture()
	{}

	Texture::Texture( UINT width, UINT height ) :
		mWidth( width ),
		mHeight( height )
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
}
