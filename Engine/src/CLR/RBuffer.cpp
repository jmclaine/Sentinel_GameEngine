#include "RBuffer.h"

namespace Sentinel { namespace Assets
{
	RBuffer::RBuffer( Buffer* buffer )
	{
		mRef = buffer;
	}

	RBuffer::~RBuffer()
	{}

	Buffer* RBuffer::GetRef()
	{
		return mRef;
	}

	////////////////////////////////

	unsigned char* RBuffer::Lock()
	{
		return (unsigned char*)mRef->Lock();
	}

	void RBuffer::Unlock()
	{
		mRef->Unlock();
	}

	void RBuffer::Release()
	{
		mRef->Release();
	}
}}
