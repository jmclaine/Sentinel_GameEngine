#pragma once

#include "Property.h"
#include "Archive.h"

namespace Sentinel { namespace Utilities
{
	public ref class WArchive
	{
		DECLARE_REF( Archive );

	public:

		WArchive();
		
		DECLARE_OP_DEREF( Archive );

		void Open( System::String^ filename, System::String^ mode );
		void Close();
	};
}}