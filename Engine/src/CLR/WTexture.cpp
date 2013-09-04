#include "WTexture.h"

namespace Sentinel { namespace Assets
{
	DEFINE_REF( Texture );

	WTexture::WTexture()
	{
		mRef = NULL;
	}

	WTexture::WTexture( Texture* texture )
	{
		mRef = texture;
	}

	////////////////////////////////

	DEFINE_OP_PTR( Texture );

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

	DEFINE_CLASS_REF_PTR( Texture );
}}
