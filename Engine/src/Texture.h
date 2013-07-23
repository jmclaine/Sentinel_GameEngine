#pragma once

#include "Common.h"

namespace Sentinel
{
	class SENTINEL_DLL Texture
	{
	public:

		std::string		mFilename;
		UINT			mWidth;
		UINT			mHeight;
		UINT			mID;

		//////////////////////////////

		Texture();
		Texture( const std::string& filename, UINT width, UINT height, UINT id );

		virtual ~Texture();

		virtual void Release();
	};
}
