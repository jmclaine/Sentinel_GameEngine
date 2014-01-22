#include "TextureGL.h"

namespace Sentinel
{
	TextureGL::TextureGL( UINT width, UINT height, GLuint id ) :
		Texture( width, height ),
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
}
