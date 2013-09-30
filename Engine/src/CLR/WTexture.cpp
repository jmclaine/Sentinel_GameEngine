#include "WTexture.h"
#include "WString.h"

using namespace Sentinel::Utilities;

namespace Sentinel { namespace Assets
{
	DEFINE_REF_PTR( Texture );

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
		System::GC::SuppressFinalize( this );
	}

	void WTexture::Delete()
	{
		SAFE_DELETE( mRef );
	}

	////////////////////////////////

	DEFINE_OP_PTR( Texture );

	void WTexture::Release()
	{
		mRef->Release();
	}

	System::String^ WTexture::Name()
	{
		return WString::Cast( mRef->Name().c_str() );
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

	DEFINE_CLASS_REF( Texture );
	DEFINE_CLASS_REF_PTR( Texture );
}}
