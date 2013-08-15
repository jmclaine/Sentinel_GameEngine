#pragma once

#include <string>

namespace Sentinel { namespace Utilities
{
	public ref class WString
	{
	private:

		std::string*	mRef;
		
	public:

		WString( std::string& str );
		~WString();
		
		char operator [] ( int index );

		void Set( std::string& str );
		void Set( System::String^ str );

		operator const char* ();
		operator System::String^ ();

		static std::string ToString( System::String^ str );
	};
}}
