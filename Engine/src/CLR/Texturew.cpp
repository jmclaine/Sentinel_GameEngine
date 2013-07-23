#include "Texturew.h"

namespace Sentinel { namespace Assets
{
	Texturew::Texturew()
	{}

	Texturew::Texturew( Texture* texture )
	{
		mRef = texture;
	}

	Texturew::Texturew( Texturew^ texture )
	{
		mRef = texture->mRef;
	}

	Texturew::~Texturew()
	{
		Release();
	}

	Texture* Texturew::GetRef()
	{
		return mRef;
	}

	void Texturew::Release()
	{
		mRef->Release();
	}
}}
