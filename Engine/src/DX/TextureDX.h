#pragma once

#include "CommonDX.h"
#include "Texture.h"

namespace Sentinel
{
	class TextureDX : public Texture
	{
	public:

		ID3D11Texture2D*			mTexture;
		ID3D11ShaderResourceView*	mResource;

		//////////////////////////////////////

		TextureDX( UINT width, UINT height, ImageFormat format, ID3D11Texture2D* texture, ID3D11ShaderResourceView* resource );
		~TextureDX();

		void Release();
	};

	class TextureCubeDX : public Texture
	{
	public:

		ID3D11Texture3D*			mTexture;
		ID3D11ShaderResourceView*	mResource;

		//////////////////////////////////////

		TextureCubeDX(UINT width, UINT height, ImageFormat format, ID3D11Texture3D* texture, ID3D11ShaderResourceView* resource);
		~TextureCubeDX();

		void Release();
	};
}
