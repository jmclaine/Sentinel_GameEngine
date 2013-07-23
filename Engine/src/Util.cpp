#ifdef WIN32
	#include <direct.h>
#endif

#include "Util.h"

namespace Sentinel
{
	void SetDirectory( const char* dest )
	{
	#ifdef WIN32
		char path[ FILENAME_MAX ];
		_getcwd( path, sizeof( path ));
		strcat_s( path, "\\" );
		strcat_s( path, dest );
		_chdir( path );
	#else
		chdir( dest );
	#endif
	}

	bool FileToBuffer( const char *filename, char*& buf )
	{
		std::ifstream file( filename, std::ios::binary );

		if( !file.is_open() )
			return false;

		file.seekg( 0, std::ios::end );
		size_t length = (size_t)( file.tellg() );
		buf = (char*)malloc( length + 1 );
		file.seekg( 0, std::ios::beg );
		file.read( buf, length );
		file.close();

		buf[ length ] = 0;
		return true;
	}

	BYTE ReadByte( std::ifstream& file )
	{
		char data[ sizeof( BYTE ) ];
		file.read( data, sizeof( BYTE ));
		return *(BYTE*)data;
	}

	int ReadInt( std::ifstream& file, bool is32bit )
	{
		if( !is32bit )
		{
			char data[ sizeof( WORD ) ];
			file.read( data, sizeof( WORD ));
			return (int)(*(WORD*)data);
		}

		char data[ sizeof( int ) ];
		file.read( data, sizeof( int ));
		return *(int*)data;
	}

	float ReadFloat( std::ifstream& file, bool is32bit )
	{
		if( !is32bit )
		{
			char data[ sizeof( short ) ];
			file.read( data, sizeof( short ));
			return FloatCompressor::Decompress( *(short*)data );
		}

		char data[ sizeof( float ) ];
		file.read( data, sizeof( float ));
		return *(float*)data;
	}

	void ReadString( std::ifstream& file, char* str, int length )
	{
		char data[ 256 ];
		file.read( data, length );
		memcpy( str, data, length );
		str[ length ] = 0;
	}

	Vector2f ReadPoint2( std::ifstream& file, bool is32bit )
	{
		float data0 = ReadFloat( file, is32bit );
		float data1 = ReadFloat( file, is32bit );
		return Vector2f( data0, data1 );
	}

	Vector3f ReadPoint3( std::ifstream& file, bool is32bit )
	{
		float data0 = ReadFloat( file, is32bit );
		float data1 = ReadFloat( file, is32bit );
		float data2 = ReadFloat( file, is32bit );
		return Vector3f( data0, data1, data2 );
	}
}
