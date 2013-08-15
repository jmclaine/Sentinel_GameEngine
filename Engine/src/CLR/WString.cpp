#include "WString.h"

namespace Sentinel { namespace Utilities
{
	WString::WString( std::string& str )
	{
		mRef = &str;
	}

	WString::~WString()
	{}

	char WString::operator [] ( int index )
	{
		return mRef->at( index );
	}

	void WString::Set( std::string& str )
	{
		mRef = &str;
	}

	void WString::Set( System::String^ str )
	{
		mRef->clear();

		for( int x = 0; x < str->Length; ++x )
			mRef->push_back( static_cast< char >(str[ x ]) );

		mRef->push_back( 0 );
	}

	WString::operator const char* ()
	{
		return mRef->c_str();
	}

	WString::operator System::String^ ()
	{
		return gcnew System::String( mRef->c_str() );
	}

	std::string WString::ToString( System::String^ str )
	{
		std::string temp;

		for( int x = 0; x < str->Length; ++x )
			temp.push_back( static_cast< char >(str[ x ]) );

		temp.push_back( 0 );

		return temp;
	}
}}
