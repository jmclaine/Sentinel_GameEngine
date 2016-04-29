#pragma once

#include "Sentinel.h"

namespace Sentinel
{
	enum class ImageFormat : BYTE
	{
		R,
		RG,
		RGB,
		RGBA,
		HDR,	// R32G32B32F
		DEPTH,
	};

	class SENTINEL_DLL Texture
	{
	protected:
		UINT mWidth;
		UINT mHeight;

		ImageFormat mFormat;

		//////////////////////////////

		Texture(UINT width, UINT height, ImageFormat format);

	public:
		virtual ~Texture();

		virtual void Release() = 0;

		UINT Width();
		UINT Height();

		const ImageFormat& Format();
	};
}
