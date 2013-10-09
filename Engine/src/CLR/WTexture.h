#pragma once

#include "Property.h"
#include "m_shared_ptr.h"
#include "Texture.h"

namespace Sentinel { namespace Assets
{
	public ref class WTexture
	{
		DECLARE_REF_SHARED( Texture );

	public:

		DECLARE_OP_SHARED( Texture );

		UINT			Width();
		UINT			Height();
	};

	DECLARE_CLASS_REF_SHARED( Texture );
}}
