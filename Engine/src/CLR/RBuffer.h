#pragma once

#include "m_shared_ptr.h"
#include "Buffer.h"

namespace Sentinel { namespace Assets
{
	public enum BufferType
	{
		VERTEX_BUFFER = 1,
		INDEX_BUFFER  = 2,

		NUM_BUFFER_TYPES
	};

	public ref class RBuffer
	{
	private:

		Buffer*		mRef;

	public:

		RBuffer( Buffer* buffer );
		~RBuffer();
		
		Buffer*		GetRef();

		////////////////////////////////

		UCHAR*		Lock();
		void		Unlock();
		void		Release();
	};
}}
