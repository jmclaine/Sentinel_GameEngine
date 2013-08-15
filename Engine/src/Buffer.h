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
	protected:

		BufferType		mType;
		UINT			mSize;
		UINT			mStride;
		UINT			mCount;

		//////////////////////////

		Buffer() {}

	public:

		virtual ~Buffer() {}

		virtual void*	Lock() = 0;
		virtual void	Unlock() = 0;
		virtual void	Release() = 0;

		BufferType		Type();
		UINT			Size();
		UINT			Stride();
		UINT			Count();
	};
}
