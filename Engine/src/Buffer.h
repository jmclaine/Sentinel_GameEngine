#pragma once

#include "Sentinel.h"
#include "RendererTypes.h"
#include "Types.h"

namespace Sentinel
{
	class Archive;
	class Renderer;

	class SENTINEL_DLL Buffer
	{
	protected:

		BufferAccess::Type mAccess;
		BufferFormat::Type mFormat;
		UINT mSize;
		UINT mStride;
		UINT mCount;

		//////////////////////////

		Buffer();

	public:

		virtual ~Buffer();

		virtual void Release() = 0;

		virtual void* Lock() = 0;
		virtual void Unlock() = 0;

		BufferFormat::Type Format();
		UINT Size();
		UINT Stride();
		UINT Count();

		///////////////////////////

		static void Save(Archive& archive, Buffer* buffer);

		static Buffer* Load(Archive& archive, Renderer* renderer);
	};
}
