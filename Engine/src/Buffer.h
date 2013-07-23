#pragma once

#include "Common.h"

namespace Sentinel
{
	enum BufferType
	{
		VERTEX_BUFFER = 1,
		INDEX_BUFFER  = 2,

		NUM_BUFFER_TYPES
	};

	class Buffer
	{
	public:

		BufferType		mType;
		UINT			mSize;
		UINT			mStride;
		UINT			mCount;

		Buffer() {}
		virtual ~Buffer() {}

		virtual void*	Lock() = 0;
		virtual void	Unlock() = 0;
		virtual void	Release() = 0;
	};
}
