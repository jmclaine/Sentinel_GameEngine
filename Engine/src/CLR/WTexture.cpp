#include "WTexture.h"

namespace Sentinel { namespace Assets
{
	WTexture::WTexture()
	{
		mRef = NULL;
	}

	WTexture::WTexture( Texture* texture )
	{
		mRef = texture;
	}

	WTexture::~WTexture()
	{
		Delete();
	}

	WTexture::!WTexture()
	{
		Delete();
	}

	void WTexture::Delete()
	{
		delete mRef;
	}

	Texture* WTexture::GetRef()
	{
		return mRef;
	}

	////////////////////////////////

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

	////////////////////////////////

	RTexture::RTexture( Texture*& texture ) :
		mRefPtr( texture )
	{
		Set( texture );
	}

	void RTexture::Set( Texture*& texture )
	{
		mRefPtr = texture;
		mRef = mRefPtr;
	}

	void RTexture::Set( WTexture^ texture )
	{
		mRefPtr = texture->GetRef();
		mRef = mRefPtr;
	}

	void RTexture::Delete()
	{}
}}
