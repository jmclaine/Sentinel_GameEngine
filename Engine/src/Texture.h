#pragma once

#include "Common.h"

namespace Sentinel
{
	class SENTINEL_DLL Texture
	{
	protected:

		std::string		mName;
		UINT			mWidth;
		UINT			mHeight;
		
		//////////////////////////////

		Texture();
		Texture( const char* name, UINT width, UINT height );

	public:

		virtual ~Texture();

		virtual void		Release();

		const std::string&	Name();
		UINT				Width();
		UINT				Height();
	};
}
