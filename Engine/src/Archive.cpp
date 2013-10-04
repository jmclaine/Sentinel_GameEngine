#include "Archive.h"
#include "FloatCompressor.h"

#include <memory.h>
#include <exception>

namespace Sentinel
{
	Archive::Archive( FILE* fp )
	{
		mFile = fp;
	}

	Archive::~Archive()
	{}

#define READ_DATA( var, type, len )\
	if( fread( var, sizeof( type ), len, mFile ) != len )\
		throw std::exception( "Failed to read from file." );

	void Archive::Read( char* data, unsigned int length )
	{
		READ_DATA( data, char, length );
	}

	void Archive::Read( unsigned char* data, unsigned int length )
	{
		READ_DATA( data, unsigned char, length );
	}

	void Archive::Read( int* data, unsigned int length, bool is32bit )
	{
		if( !is32bit )
		{
			for( unsigned int x = 0; x < length; ++x )
			{
				short d;
				READ_DATA( &d, short, 1 );
				data[ x ] = d;
			}
		}
		else
		{
			READ_DATA( data, int, length );
		}
	}

	void Archive::Read( unsigned int* data, unsigned int length, bool is32bit )
	{
		if( !is32bit )
		{
			for( unsigned int x = 0; x < length; ++x )
			{
				unsigned short d;
				READ_DATA( &d, unsigned short, 1 );
				data[ x ] = d;
			}
		}
		else
		{
			READ_DATA( data, unsigned int, length );
		}
	}

	void Archive::Read( long* data, unsigned int length, bool is32bit )
	{
		if( !is32bit )
		{
			for( unsigned long x = 0; x < length; ++x )
			{
				short d;
				READ_DATA( &d, short, 1 );
				data[ x ] = d;
			}
		}
		else
		{
			READ_DATA( data, long, length );
		}
	}

	void Archive::Read( unsigned long* data, unsigned int length, bool is32bit )
	{
		if( !is32bit )
		{
			for( unsigned long x = 0; x < length; ++x )
			{
				unsigned short d;
				READ_DATA( &d, unsigned short, 1 );
				data[ x ] = d;
			}
		}
		else
		{
			READ_DATA( data, unsigned long, length );
		}
	}

	void Archive::Read( float* data, unsigned int length, bool is32bit )
	{
		if( !is32bit )
		{
			for( unsigned int x = 0; x < length; ++x )
			{
				short d;
				READ_DATA( &d, short, 1 );
				data[ x ] = FloatCompressor::Decompress( d );
			}
		}
		else
		{
			READ_DATA( data, float, length );
		}
	}

	void Archive::Read( double* data, unsigned int length )
	{
		READ_DATA( data, double, length );
	}

	void Archive::Read( std::string* data )
	{
		data->clear();
		
		char c = 0;
		Read( &c );

		while( c != 0 )
		{
			data->push_back( c );
			Read( &c );
		}
	}
	
	////////////////////////////////////////////////////////////////////////////////

#define WRITE_DATA( var, type, len )\
	if( fwrite( var, sizeof( type ), len, mFile ) != len )\
		throw std::exception( "Failed to write to file." );

	void Archive::Write( const char* data, unsigned int length )
	{
		WRITE_DATA( data, char, length );
	}

	void Archive::Write( const unsigned char* data,	unsigned int length )
	{
		WRITE_DATA( data, unsigned char, length );
	}

	void Archive::Write( const int* data, unsigned int length, bool is32bit )
	{
		if( !is32bit )
		{
			for( unsigned int x = 0; x < length; ++x )
			{
				short d = (short)data[ x ];
				WRITE_DATA( &d, short, 1 );
			}
		}
		else
		{
			WRITE_DATA( data, int, length );
		}
	}

	void Archive::Write( const unsigned int* data, unsigned int length, bool is32bit )
	{
		if( !is32bit )
		{
			for( unsigned int x = 0; x < length; ++x )
			{
				unsigned short d = (unsigned short)data[ x ];
				WRITE_DATA( &d, unsigned short, 1 );
			}
		}
		else
		{
			WRITE_DATA( data, unsigned int, length );
		}
	}

	void Archive::Write( const long* data, unsigned int length, bool is32bit )
	{
		if( !is32bit )
		{
			for( unsigned long x = 0; x < length; ++x )
			{
				short d = (short)data[ x ];
				WRITE_DATA( &d, short, 1 );
			}
		}
		else
		{
			WRITE_DATA( data, long, length );
		}
	}

	void Archive::Write( const unsigned long* data, unsigned int length, bool is32bit )
	{
		if( !is32bit )
		{
			for( unsigned long x = 0; x < length; ++x )
			{
				unsigned short d = (unsigned short)data[ x ];
				WRITE_DATA( &d, unsigned short, 1 );
			}
		}
		else
		{
			WRITE_DATA( data, unsigned long, length );
		}
	}

	void Archive::Write( const float* data, unsigned int length, bool is32bit )
	{
		if( !is32bit )
		{
			for( unsigned int x = 0; x < length; ++x )
			{
				short d = FloatCompressor::Compress( data[ x ] );
				WRITE_DATA( &d, short, 1 );
			}
		}
		else
		{
			WRITE_DATA( data, float, length );
		}
	}

	void Archive::Write( const double* data, unsigned int length )
	{
		WRITE_DATA( data, double, length );
	}

	void Archive::Write( const std::string* data )
	{
		Write( data->c_str(), (unsigned int)data->size() );

		char end = 0;
		Write( &end );
	}

	/////////////////////////////////////////////////////////////

	bool Archive::ToBuffer( const char *filename, char*& buf )
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
}
