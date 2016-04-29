#pragma once

#include "Sentinel.h"

namespace Sentinel
{
	class Archive;
	class Renderer;

	/////////////////////////////////

	enum class BufferFormat : BYTE
	{
		UNKNOWN,

		VERTEX,
		INDEX,
	};

	/////////////////////////////////

	enum class BufferAccess : BYTE
	{
		READ_WRITE,
		WRITE,
	};

	/////////////////////////////////

	class SENTINEL_DLL Buffer
	{
	protected:
		BufferAccess mAccess;
		BufferFormat mFormat;
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

		BufferFormat Format();
		UINT Size();
		UINT Stride();
		UINT Count();

		///////////////////////////

		static void Save(Archive& archive, Buffer* buffer);

		static Buffer* Load(Archive& archive, Renderer* renderer);
	};
}
