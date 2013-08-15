#include "Buffer.h"

namespace Sentinel
{
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
