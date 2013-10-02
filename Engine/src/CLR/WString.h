#pragma once

#include <string>
#include <msclr\marshal_cppstd.h>

using namespace System::Runtime::InteropServices;

namespace Sentinel { namespace Utilities
{
	public ref class WString
	{
	protected:

		const char*		mRef;

	public:

		const char*		GetRef();

	public:

		WString( const char* str );
		WString( System::String^ str );
		~WString();
		!WString();

	protected:

		virtual void Delete();

	public:

		char operator [] ( int index );

		operator const char* ();
		operator System::String^ ();

		virtual System::String^ ToString() override;

		static const char* Alloc( System::String^ str );
		static void Free( char* str );

		static std::string Cast( System::String^ str );
		static System::String^ Cast( const char* str );
		static System::String^ Cast( const std::string& str );
	};
}}
