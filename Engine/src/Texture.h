#pragma once

#include "Common.h"

namespace Sentinel
{
	class SENTINEL_DLL Texture
	{
	protected:

		UINT			mWidth;
		UINT			mHeight;
		
		//////////////////////////////

		Texture();
		Texture( UINT width, UINT height );

	public:

		virtual ~Texture();

		virtual void		Release() = 0;

		UINT				Width();
		UINT				Height();
	};
}
