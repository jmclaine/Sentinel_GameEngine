#include "Buffer.h"

namespace Sentinel
{
	Buffer::Buffer() :
		mType( NULL_BUFFER ), mSize( 0 ), mStride( 0 ), mCount( 0 )
	{}

	Buffer::~Buffer()
	{
		mType	= NULL_BUFFER;
		mSize	= 0;
		mStride = 0;
		mCount	= 0;
	}

	BufferType Buffer::Type()
	{
		return mType;
	}

	UINT Buffer::Size()
	{
		return mSize;
	}

	UINT Buffer::Stride()
	{
		return mStride;
	}

	UINT Buffer::Count()
	{
		return mCount;
	}
}
