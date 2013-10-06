#include "WArchive.h"
#include "WString.h"

namespace Sentinel { namespace Utilities
{
	DEFINE_REF( Archive );

	WArchive::WArchive()
	{
		mRef = new Archive();
	}

	DEFINE_OP_DEREF( Archive );

	void WArchive::Open( System::String^ filename, System::String^ mode )
	{
		mRef->Open( WString::Cast( filename ).c_str(), WString::Cast( mode ).c_str() );
	}

	void WArchive::Close()
	{
		mRef->Close();
	}
}}
