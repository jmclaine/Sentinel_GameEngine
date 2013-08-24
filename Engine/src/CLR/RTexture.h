#pragma once

#include "m_shared_ptr.h"
#include "Texture.h"

namespace Sentinel { namespace Assets
{
	public ref class RTexture
	{
	private:

		Texture*			mRef;

	public:

		RTexture( Texture* texture );
		~RTexture();
		
		Texture*			GetRef();

		////////////////////////////////

		void				Release();

		System::String^		Filename();
		UINT				Width();
		UINT				Height();
		UINT				ID();
	};
}}
