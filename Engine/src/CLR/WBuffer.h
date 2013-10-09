#pragma once

#include "Property.h"
#include "Buffer.h"

using namespace Sentinel;

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

		System::IntPtr^ Lock();
		void		Unlock();
		
		Sentinel::Assets::BufferType Type();
		UINT		Size();
		UINT		Stride();
		UINT		Count();
	};

	DECLARE_CLASS_REF( Buffer );
	DECLARE_CLASS_REF_PTR( Buffer );
}}
