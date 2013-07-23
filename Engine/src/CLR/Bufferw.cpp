#include "Bufferw.h"

namespace Sentinel { namespace Assets
{
	Bufferw::Bufferw()
	{}

	Bufferw::Bufferw( Buffer* buffer )
	{
		mRef = buffer;
	}

	Bufferw::Bufferw( Bufferw^ buffer )
	{
		mRef = buffer->mRef;
	}

	Bufferw::~Bufferw()
	{
		Release();
	}

	Buffer* Bufferw::GetRef()
	{
		return mRef;
	}

	unsigned char* Bufferw::Lock()
	{
		return (unsigned char*)mRef->Lock();
	}

	void Bufferw::Unlock()
	{
		mRef->Unlock();
	}

	void Bufferw::Release()
	{
		mRef->Release();
	}
}}
