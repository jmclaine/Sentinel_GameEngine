#include "FileIO.h"
#include "FloatCompressor.h"

namespace Sentinel
{
	bool FileIO::ToBuffer( const char *filename, char*& buf )
	{
		FILE* file = fopen( filename, "rb" );

		if( !file )
			return false;

		fseek( file, 0, SEEK_END );
		size_t length = (size_t)( ftell( file ));
		rewind( file );

		buf = (char*)malloc( length + 1 );
		fread( buf, sizeof( char ), length, file );
		fclose( file );

		buf[ length ] = 0;
		return true;
	}

	////////////////////////////////////////////////////////////////////////////////

#define READ_DATA( var, type )\
	if( fread( &var, sizeof( type ), 1, file ) != 1 )\
		throw std::exception( "Failed to read from file." );

	void FileIO::Read( FILE* file, unsigned char& data )
	{
		READ_DATA( data, unsigned char );
	}

	void FileIO::Read( FILE* file, int& data, bool is32bit )
	{
		if( !is32bit )
		{
			short d = 0;
			READ_DATA( d, short );
			data = (int)d;
		}
		else
		{
			READ_DATA( data, int );
		}
	}

	void FileIO::Read( FILE* file, unsigned int& data, bool is32bit )
	{
		if( !is32bit )
		{
			unsigned short d;
			READ_DATA( d, short );
			data = (unsigned int)d;
		}
		else
		{
			READ_DATA( data, unsigned int );
		}
	}

	void FileIO::Read( FILE* file, float& data, bool is32bit )
	{
		if( !is32bit )
		{
			short d;
			READ_DATA( d, short );
			data = FloatCompressor::Decompress( d );
		}
		else
		{
			READ_DATA( data, float );
		}
	}

	void FileIO::Read( FILE* file, char* data, int length )
	{
		char d[ 256 ];

		if( fread( d, sizeof( char ), length, file ) != length )
			throw std::exception( "Failed to read from file." );

		memcpy( data, d, length );
		data[ length ] = 0;
	}

	void FileIO::Read( FILE* file, Vector2f& data,	bool is32bit )
	{
		for( int x = 0; x < 2; ++x )
		{
			float d;
			Read( file, d, is32bit );
			data[ x ] = d;
		}
	}

	void FileIO::Read( FILE* file, Vector3f& data,	bool is32bit )
	{
		for( int x = 0; x < 3; ++x )
		{
			float d;
			Read( file, d, is32bit );
			data[ x ] = d;
		}
	}

	void FileIO::Read( FILE* file, Matrix4f& data, bool is32bit )
	{
		for( int z = 0; z < 4; ++z )
			for( int w = 0; w < 4; ++w )
				Read( file, data[ (z<<2)+w ], is32bit );
	}

	////////////////////////////////////////////////////////////////////////////////

#define WRITE_DATA( var, type )\
	if( fwrite( &var, sizeof( type ), 1, file ) != 1 )\
		throw std::exception( "Failed to write to file." );

	void FileIO::Write( FILE* file, const unsigned char& data )
	{
		WRITE_DATA( data, unsigned char );
	}

	void FileIO::Write( FILE* file, const int& data, bool is32bit )
	{
		if( !is32bit )
		{
			short d = (short)data;
			WRITE_DATA( d, short );
		}
		else
		{
			WRITE_DATA( data, int );
		}
	}

	void FileIO::Write( FILE* file, const unsigned int& data, bool is32bit )
	{
		if( !is32bit )
		{
			unsigned short d = (unsigned short)data;
			WRITE_DATA( d, short );
		}
		else
		{
			WRITE_DATA( data, unsigned int );
		}
	}

	void FileIO::Write( FILE* file, const float& data, bool is32bit )
	{
		if( !is32bit )
		{
			short d = FloatCompressor::Compress( data );
			WRITE_DATA( d, short );
		}
		else
		{
			WRITE_DATA( data, float );
		}
	}

	void FileIO::Write( FILE* file, const char* str )
	{
		if( fwrite( str, sizeof( char ), strlen( str ), file ) != strlen( str ))
			throw std::exception( "Failed to read from file." );
	}

	void FileIO::Write( FILE* file, const Vector2f& data, bool is32bit )
	{
		for( int x = 0; x < 2; ++x )
			Write( file, data[ x ], is32bit );
	}

	void FileIO::Write( FILE* file, const Vector3f& data, bool is32bit )
	{
		for( int x = 0; x < 3; ++x )
			Write( file, data[ x ], is32bit );
	}

	void FileIO::Write( FILE* file, const Matrix4f& data, bool is32bit )
	{
		for( int z = 0; z < 4; ++z )
			for( int w = 0; w < 4; ++w )
				Write( file, data[ (z<<2)+w ], is32bit );
	}
}
