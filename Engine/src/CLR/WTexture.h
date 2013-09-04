#pragma once

#include "Property.h"
#include "m_shared_ptr.h"
#include "Texture.h"

using namespace System;

namespace Sentinel { namespace Assets
{
	public ref class WTexture
	{
		DECLARE_REF( Texture );

	protected:

		WTexture();

	public:

		WTexture( Texture* texture );
		
		////////////////////////////////

		DECLARE_OP_PTR( Texture );

		void		Release();

		String^		Filename();
		UINT		Width();
		UINT		Height();
	};

	DECLARE_CLASS_REF_PTR( Texture );
}}
