#pragma once

#include "Property.h"
#include "WRendererTypes.h"
#include "Buffer.h"

namespace Sentinel { namespace Wrapped
{
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
		
		BufferType	Type();
		UINT		Size();
		UINT		Stride();
		UINT		Count();
	};

	DECLARE_CLASS_REF( Buffer );
	DECLARE_CLASS_REF_PTR( Buffer );
}}
