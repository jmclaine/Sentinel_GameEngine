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

	public ref class Bufferw
	{
	private:

		Buffer*		mRef;

	public:

		Bufferw();
		Bufferw( Buffer* buffer );
		Bufferw( Bufferw^ buffer );
		~Bufferw();

		Buffer*			GetRef();

		unsigned char*	Lock();
		void			Unlock();
		void			Release();
	};
}}
