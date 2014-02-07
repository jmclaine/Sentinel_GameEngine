#include "TextureGL.h"

namespace Sentinel
{
	TextureGL::TextureGL( UINT width, UINT height, ImageFormatType format, GLuint id ) :
		Texture( width, height, format ),
		mID( id )
	{}

	TextureGL::~TextureGL()
	{
		Release();
	}

	void TextureGL::Release()
	{
		glDeleteTextures( 1, &mID );
	}

	GLuint TextureGL::ID()
	{
		return mID;
	}
}
