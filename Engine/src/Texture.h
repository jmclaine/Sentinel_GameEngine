#pragma once

#include "Common.h"

namespace Sentinel
{
	class SENTINEL_DLL Texture
	{
	protected:

		std::string		mFilename;
		UINT			mWidth;
		UINT			mHeight;
		
		//////////////////////////////

		Texture();
		Texture( const char* filename, UINT width, UINT height );

	public:

		virtual ~Texture();

		virtual void		Release();

		const std::string&	Filename();
		UINT				Width();
		UINT				Height();
	};
}
