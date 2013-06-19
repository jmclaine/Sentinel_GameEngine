#pragma once

#include <string>

typedef unsigned int UINT;

namespace Sentinel
{
	class Texture
	{
	public:

		std::string	mFilename;
		UINT		mWidth;
		UINT		mHeight;
		UINT		mID;

		//////////////////////////////

		Texture() {}
		Texture( std::string filename, UINT width, UINT height, UINT id ) :
			mFilename( filename ),
			mWidth( width ),
			mHeight( height ),
			mID( id )
		{}

		virtual ~Texture()
		{
			Release();
		}

		virtual void Release()
		{}
	};
}