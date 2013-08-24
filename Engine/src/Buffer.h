#pragma once

#include "Common.h"

namespace Sentinel
{
	enum BufferType
	{
		NULL_BUFFER,
		VERTEX_BUFFER,
		INDEX_BUFFER,

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

		Buffer();

	public:

		virtual ~Buffer();

		virtual void*	Lock() = 0;
		virtual void	Unlock() = 0;
		virtual void	Release() = 0;

		BufferType		Type();
		UINT			Size();
		UINT			Stride();
		UINT			Count();
	};
}
