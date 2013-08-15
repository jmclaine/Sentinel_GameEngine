#pragma once

#include "m_shared_ptr.h"
#include "Texture.h"

namespace Sentinel { namespace Assets
{
	public ref class WTexture
	{
	private:

		Texture*			mRef;

	public:

		WTexture( Texture* texture );
		WTexture( WTexture^ texture );
		~WTexture();
		
		Texture*			GetRef();

		void				Release();

		System::String^		Filename();
		UINT				Width();
		UINT				Height();
		UINT				ID();
	};
}}
