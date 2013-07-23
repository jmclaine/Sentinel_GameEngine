#pragma once

#include "Texture.h"

namespace Sentinel { namespace Assets
{
	public ref class Texturew
	{
	private:

		Texture*	mRef;

	public:

		Texturew();
		Texturew( Texture* texture );
		Texturew( Texturew^ texture );
		~Texturew();

		Texture* GetRef();

		void Release();
	};
}}
