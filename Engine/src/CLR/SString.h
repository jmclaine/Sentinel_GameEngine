#pragma once

namespace Sentinel
{
	public ref class SString
	{
		char* data;

	public:

		SString( System::String^ str )
		{
			data = new char[ str->Length ];

			for( int x = 0; x < str->Length; ++x )
				data[ x ] = static_cast< char >(str[ x ]);
		}

		~SString()
		{
			delete[] data;
		}

		operator const char* ()
		{
			return data;
		}
	};
}
