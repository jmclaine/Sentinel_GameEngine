#include "Texture.h"

namespace Sentinel
{
	Texture::Texture()
	{}

	Texture::Texture( const char* filename, UINT width, UINT height ) :
		mFilename( filename ),
		mWidth( width ),
		mHeight( height )
	{}

	Texture::~Texture()
	{
		Release();
	}

	void Texture::Release()
	{}

	const std::string& Texture::Filename()
	{
		return mFilename;
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
