#include "WString.h"

namespace Sentinel { namespace Utilities
{
	WString::WString( const char* str )
	{
		mRef = str;
	}

	WString::WString( System::String^ str )
	{
		mRef = (const char*)Marshal::StringToHGlobalAnsi( str ).ToPointer();
	}

	WString::~WString()
	{
		Release();
	}

	WString::!WString()
	{
		Release();
	}

	const char* WString::GetRef()
	{
		return mRef;
	}

	void WString::Release()
	{
		Marshal::FreeHGlobal( System::IntPtr( (char*)mRef ));
	}

	char WString::operator [] ( int index )
	{
		return mRef[ index ];
	}

	WString::operator const char* ()
	{
		return mRef;
	}

	WString::operator System::String^ ()
	{
		return gcnew System::String( mRef );
	}

	System::String^ WString::ToString()
	{
		return this;
	}

	const char* WString::Alloc( System::String^ str )
	{
		return (const char*)Marshal::StringToHGlobalAnsi( str ).ToPointer();
	}

	void WString::Free( char* str )
	{
		if( str )
			Marshal::FreeHGlobal( System::IntPtr( str ));
	}

	std::string WString::Cast( System::String^ str )
	{
		return msclr::interop::marshal_as< std::string >(str);
	}

	System::String^ WString::Cast( const char* str )
	{
		return msclr::interop::marshal_as< System::String^ >(str);
	}

	System::String^ WString::Cast( const std::string& str )
	{
		return msclr::interop::marshal_as< System::String^ >(str);
	}
}}
