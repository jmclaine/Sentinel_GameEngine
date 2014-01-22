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

		TextureDX( UINT width, UINT height, ID3D11Texture2D* texture, ID3D11ShaderResourceView* resource );
		~TextureDX();

		void Release();
	};
}
