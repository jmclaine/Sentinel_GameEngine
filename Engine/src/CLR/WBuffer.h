#pragma once

#include "m_shared_ptr.h"
#include "Buffer.h"

namespace Sentinel { namespace Assets
{
	public enum class BufferType
	{
		VERTEX_BUFFER = 1,
		INDEX_BUFFER  = 2,

		NUM_BUFFER_TYPES
	};

	public ref class WBuffer
	{
	private:

		Buffer*		mRef;

	public:

		WBuffer( Buffer* buffer );
		~WBuffer();
		
		Buffer*		GetRef();

		////////////////////////////////

		UCHAR*		Lock();
		void		Unlock();
		void		Release();
	};
}}
