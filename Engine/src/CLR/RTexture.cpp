#include "RTexture.h"

namespace Sentinel { namespace Assets
{
	RTexture::RTexture( Texture* texture )
	{
		mRef = texture;
	}

	RTexture::~RTexture()
	{}

	Texture* RTexture::GetRef()
	{
		return mRef;
	}

	////////////////////////////////

	void RTexture::Release()
	{
		mRef->Release();
	}

	System::String^ RTexture::Filename()
	{
		return gcnew System::String( mRef->Filename().c_str() );
	}

	UINT RTexture::Width()
	{
		return mRef->Width();
	}

	UINT RTexture::Height()
	{
		return mRef->Height();
	}

	UINT RTexture::ID()
	{
		return mRef->ID();
	}
}}
