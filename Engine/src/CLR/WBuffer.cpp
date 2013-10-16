#include "WBuffer.h"

namespace Sentinel { namespace Wrapped
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

	System::IntPtr^ WBuffer::Lock()
	{
		return gcnew System::IntPtr( mRef->Lock() );
	}

	void WBuffer::Unlock()
	{
		mRef->Unlock();
	}

	BufferType WBuffer::Type()
	{
		return (BufferType)mRef->Type();
	}

	UINT WBuffer::Size()
	{
		 return mRef->Size();
	}

	UINT WBuffer::Stride()
	{
		return mRef->Stride();
	}

	UINT WBuffer::Count()
	{
		return mRef->Count();
	}

	////////////////////////////////

	DEFINE_CLASS_REF( Buffer );
	DEFINE_CLASS_REF_PTR( Buffer );
}}
