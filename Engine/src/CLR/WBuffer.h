#pragma once

#include "Property.h"
#include "Buffer.h"

namespace Sentinel { namespace Assets
{
	public enum class BufferType
	{
		VERTEX = 1,
		INDEX  = 2,

		NUM_BUFFER_TYPES
	};

	public ref class WBuffer
	{
		DECLARE_REF( Buffer );

	protected:

		WBuffer();

	public:

		WBuffer( Buffer* buffer );

		DECLARE_OP_PTR( Buffer );
		
		////////////////////////////////

		UCHAR*		Lock();
		void		Unlock();
		void		Release();
	};

	DECLARE_CLASS_REF_PTR( Buffer );
}}
