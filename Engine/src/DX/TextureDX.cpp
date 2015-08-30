#include "TextureDX.h"
#include "Memory.h"

namespace Sentinel
{
	TextureDX::TextureDX(
		UINT width, UINT height, 
		ImageFormat::Type format, 
		ID3D11Texture2D* texture, 
		ID3D11ShaderResourceView* resource) :

		Texture(width, height, format),
		mTexture(texture),
		mResource(resource)
	{}

	TextureDX::~TextureDX()
	{
		Release();
	}

	void TextureDX::Release()
	{
		SAFE_RELEASE_PTR(mTexture);
		SAFE_RELEASE_PTR(mResource);
	}
}
