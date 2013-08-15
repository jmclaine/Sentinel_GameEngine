#include "WBuffer.h"

namespace Sentinel { namespace Assets
{
	WBuffer::WBuffer( Buffer* buffer )
	{
		mRef = buffer;
	}

	WBuffer::WBuffer( WBuffer^ buffer )
	{
		mRef = buffer->mRef;
	}

	WBuffer::~WBuffer()
	{}

	Buffer* WBuffer::GetRef()
	{
		return mRef;
	}

	unsigned char* WBuffer::Lock()
	{
		return (unsigned char*)mRef->Lock();
	}

	void WBuffer::Unlock()
	{
		mRef->Unlock();
	}

	void WBuffer::Release()
	{
		mRef->Release();
	}
}}
