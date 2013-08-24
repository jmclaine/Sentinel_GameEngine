#include "RFloat.h"

namespace Sentinel { namespace Utilities
{
	RFloat::RFloat( float* value )
	{
		mRef = value;
	}

	RFloat::RFloat( RFloat^ value )
	{
		Set( value );
	}

	RFloat::RFloat( RFloat% value )
	{
		mRef = value.mRef;
	}

	RFloat::~RFloat()
	{}

	void RFloat::Set( float value )
	{
		*mRef = value;
	}

	void RFloat::Set( RFloat^ value )
	{
		*mRef = *value->mRef;
	}

	RFloat::operator float( RFloat^ value )
	{
		return *value->mRef;
	}
}}
