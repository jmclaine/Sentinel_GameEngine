#pragma once
/*
A referenced string.
Used for public member variables of classes.
Allows setting as if variable were public.
*/
#include <string>

namespace Sentinel { namespace Utilities
{
	public ref class RString
	{
	private:

		std::string*	mRef;
		
	public:

		RString( std::string& str );
		~RString();
		
		char operator [] ( int index );

		void Set( std::string& str );
		void Set( System::String^ str );

		operator const char* ();
		operator System::String^ ();

		static std::string ToString( System::String^ str );
	};
}}
