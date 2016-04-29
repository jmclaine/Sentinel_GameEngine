#pragma once

#include "CommonGL.h"
#include "Texture.h"

namespace Sentinel
{
	class TextureGL : public Texture
	{
	private:

		GLuint mID;

	public:

		TextureGL(UINT width, UINT height, ImageFormat format, GLuint id);
		~TextureGL();

		void Release();

		GLuint ID();
	};
}
