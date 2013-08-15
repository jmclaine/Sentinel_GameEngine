#include "WTexture.h"

namespace Sentinel { namespace Assets
{
	WTexture::WTexture( Texture* texture )
	{
		mRef = texture;
	}

	WTexture::WTexture( WTexture^ texture )
	{
		mRef = texture->mRef;
	}

	WTexture::~WTexture()
	{}

	Texture* WTexture::GetRef()
	{
		return mRef;
	}

	void WTexture::Release()
	{
		mRef->Release();
	}

	System::String^ WTexture::Filename()
	{
		return gcnew System::String( mRef->Filename().c_str() );
	}

	UINT WTexture::Width()
	{
		return mRef->Width();
	}

	UINT WTexture::Height()
	{
		return mRef->Height();
	}

	UINT WTexture::ID()
	{
		return mRef->ID();
	}
}}
