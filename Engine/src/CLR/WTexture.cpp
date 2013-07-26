#include "WTexture.h"

namespace Sentinel { namespace Assets
{
	WTexture::WTexture()
	{}

	WTexture::WTexture( Texture* texture )
	{
		mRef = texture;
	}

	WTexture::WTexture( WTexture^ texture )
	{
		mRef = texture->mRef;
	}

	WTexture::~WTexture()
	{
		Release();
	}

	Texture* WTexture::GetRef()
	{
		return mRef;
	}

	void WTexture::Release()
	{
		mRef->Release();
	}
}}
