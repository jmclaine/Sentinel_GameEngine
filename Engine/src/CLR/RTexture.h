#pragma once

#include "m_shared_ptr.h"
#include "Texture.h"

using namespace System;

namespace Sentinel { namespace Assets
{
	public ref class RTexture
	{
	private:

		Texture*	mRef;

	public:

		RTexture( Texture* texture );
		~RTexture();
		
		Texture*	GetRef();

		////////////////////////////////

		void		Release();

		String^		Filename();
		UINT		Width();
		UINT		Height();
		UINT		ID();
	};
}}
