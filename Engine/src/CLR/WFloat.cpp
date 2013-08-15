#include "WFloat.h"

namespace Sentinel { namespace Utilities
{
	WFloat::WFloat( float* value )
	{
		mRef = value;
	}

	WFloat::WFloat( WFloat^ value )
	{
		mRef = value->mRef;
	}

	WFloat::WFloat( WFloat% value )
	{
		mRef = value.mRef;
	}

	WFloat::~WFloat()
	{
		delete mRef;
	}

	void WFloat::Set( WFloat^ value )
	{
		*mRef = *value->mRef;
	}

	void WFloat::Set( float value )
	{
		*mRef = value;
	}

	WFloat::operator float( WFloat^ value )
	{
		return *value->mRef;
	}

	WFloat::operator WFloat^( float value )
	{
		return gcnew WFloat( new float( value ));
	}
}}
