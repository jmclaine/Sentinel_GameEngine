#include "Texture.h"

namespace Sentinel
{
	Texture::Texture()
	{}

	Texture::Texture( const std::string& filename, UINT width, UINT height, UINT id ) :
		mFilename( filename ),
		mWidth( width ),
		mHeight( height ),
		mID( id )
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

	UINT Texture::ID()
	{
		return mID;
	}
}
