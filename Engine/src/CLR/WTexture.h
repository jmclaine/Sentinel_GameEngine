#pragma once

#include "Texture.h"

namespace Sentinel { namespace Assets
{
	public ref class WTexture
	{
	private:

		Texture*	mRef;

	public:

		WTexture();
		WTexture( Texture* texture );
		WTexture( WTexture^ texture );
		~WTexture();

		Texture* GetRef();

		void Release();
	};
}}
