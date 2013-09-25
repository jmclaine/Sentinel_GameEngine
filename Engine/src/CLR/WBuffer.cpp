#include "WBuffer.h"

using namespace Sentinel;

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

	System::IntPtr^ WBuffer::Lock()
	{
		return gcnew System::IntPtr( mRef->Lock() );
	}

	void WBuffer::Unlock()
	{
		mRef->Unlock();
	}

	void WBuffer::Release()
	{
		mRef->Release();
	}

	Sentinel::Assets::BufferType WBuffer::Type()
	{
		return (Sentinel::Assets::BufferType)mRef->Type();
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
