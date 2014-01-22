#pragma once

#include "CommonGL.h"
#include "Texture.h"

namespace Sentinel
{
	class TextureGL : public Texture
	{
	public:

		GLuint	mID;

		///////////////////////////////////

		TextureGL( UINT width, UINT height, GLuint id );
		~TextureGL();

		void	Release();
	};
}
