#pragma once

#include "Common.h"
#include "RendererTypes.h"

namespace Sentinel
{
	class SENTINEL_DLL Texture
	{
	protected:

		UINT				mWidth;
		UINT				mHeight;

		ImageFormatType		mFormat;

		//////////////////////////////

		Texture( UINT width, UINT height, ImageFormatType format );

	public:

		virtual ~Texture();

		virtual void		Release() = 0;

		UINT				Width();
		UINT				Height();

		const ImageFormatType& Format();
	};
}
