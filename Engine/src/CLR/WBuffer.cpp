#include "WBuffer.h"

namespace Sentinel { namespace Assets
{
	DEFINE_REF( Buffer );

	WBuffer::WBuffer()
	{
		mRef = NULL;
	}

	WBuffer::WBuffer( Buffer* buffer )
	{
		mRef = buffer;
	}

	DEFINE_OP_PTR( Buffer );

	////////////////////////////////

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

	////////////////////////////////

	DEFINE_CLASS_REF_PTR( Buffer );
}}
