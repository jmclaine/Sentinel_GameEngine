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
}
