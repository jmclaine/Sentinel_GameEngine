#include "Texture.h"

namespace Sentinel
{
	Texture::Texture()
	{}

	Texture::Texture( const char* name, UINT width, UINT height ) :
		mName( name ),
		mWidth( width ),
		mHeight( height )
	{}

	Texture::~Texture()
	{
		Release();
	}

	void Texture::Release()
	{}

	const std::string& Texture::Name()
	{
		return mName;
	}

	UINT Texture::Width()
	{
		return mWidth;
	}

	UINT Texture::Height()
	{
		return mHeight;
	}
}
