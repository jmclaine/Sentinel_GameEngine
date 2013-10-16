#pragma once

#include "Common.h"
#include "RendererTypes.h"

namespace Sentinel
{
	class Renderer;

	class SENTINEL_DLL Buffer
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
