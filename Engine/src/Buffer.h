#pragma once

#include "Common.h"
#include "RendererTypes.h"

namespace Sentinel
{
	class Archive;
	class Renderer;

	class SENTINEL_DLL Buffer
	{
	protected:

		BufferAccess	mAccess;

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

		///////////////////////////

		static void		Save( Archive& archive, Buffer* buffer );
		
		static Buffer*	Load( Archive& archive, Renderer* renderer );
	};
}
