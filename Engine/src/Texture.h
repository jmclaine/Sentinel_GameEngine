#pragma once

#include "Sentinel.h"
#include "Types.h"
#include "RendererTypes.h"

namespace Sentinel
{
	class SENTINEL_DLL Texture
	{
	protected:

		UINT mWidth;
		UINT mHeight;

		ImageFormat::Type mFormat;

		//////////////////////////////

		Texture(UINT width, UINT height, ImageFormat::Type format);

	public:

		virtual ~Texture();

		virtual void Release() = 0;

		UINT Width();
		UINT Height();

		const ImageFormat::Type& Format();
	};
}
