#include "RString.h"

namespace Sentinel { namespace Utilities
{
	RString::RString( std::string& str )
	{
		mRef = &str;
	}

	RString::~RString()
	{}

	char RString::operator [] ( int index )
	{
		return mRef->at( index );
	}

	void RString::Set( std::string& str )
	{
		mRef = &str;
	}

	void RString::Set( System::String^ str )
	{
		mRef->clear();

		for( int x = 0; x < str->Length; ++x )
			mRef->push_back( static_cast< char >(str[ x ]) );

		mRef->push_back( 0 );
	}

	RString::operator const char* ()
	{
		return mRef->c_str();
	}

	RString::operator System::String^ ()
	{
		return gcnew System::String( mRef->c_str() );
	}

	std::string RString::ToString( System::String^ str )
	{
		std::string temp;

		for( int x = 0; x < str->Length; ++x )
			temp.push_back( static_cast< char >(str[ x ]) );

		temp.push_back( 0 );

		return temp;
	}

	System::String^ RString::ToString()
	{
		return gcnew System::String( mRef->c_str() );
	}
}}
