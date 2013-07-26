#pragma once

#include "Buffer.h"

namespace Sentinel { namespace Assets
{
	public enum BufferType
	{
		VERTEX_BUFFER = 1,
		INDEX_BUFFER  = 2,

		NUM_BUFFER_TYPES
	};

	public ref class WBuffer
	{
	private:

		Buffer*	mRef;

	public:

		WBuffer();
		WBuffer( Buffer* buffer );
		WBuffer( WBuffer^ buffer );
		~WBuffer();

		Buffer*		GetRef();

		UCHAR*		Lock();
		void		Unlock();
		void		Release();
	};
}}
