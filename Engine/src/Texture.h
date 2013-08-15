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
		UINT			mID;

		//////////////////////////////

		Texture();
		Texture( const std::string& filename, UINT width, UINT height, UINT id );

	public:

		virtual ~Texture();

		virtual void		Release();

		const std::string&	Filename();
		UINT				Width();
		UINT				Height();
		UINT				ID();
	};
}
