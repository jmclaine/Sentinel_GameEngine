#include "WTexture.h"
#include "WString.h"

using namespace Sentinel::Wrapped;

namespace Sentinel { namespace Wrapped
{
	DEFINE_REF_SHARED( Texture );

	DEFINE_OP_SHARED( Texture );

	////////////////////////////////

	UINT WTexture::Width()
	{
		return mRef->Width();
	}

	UINT WTexture::Height()
	{
		return mRef->Height();
	}

	////////////////////////////////

	DEFINE_CLASS_REF_SHARED( Texture );
}}
