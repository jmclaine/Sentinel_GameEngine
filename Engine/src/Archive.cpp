#include "Archive.h"
#include "FloatCompressor.h"

#include <memory.h>
#include <exception>

namespace Sentinel
{
	Archive::Archive()
	{
		mFile = NULL;
	}

	Archive::~Archive()
	{
		if( mFile != NULL )
		{
			fclose( mFile );
			mFile = NULL;
		}
	}

	bool Archive::Open( const char* filename, const char* mode )
	{
		mFile = fopen( filename, mode );

		return (mFile != NULL);
	}

	void Archive::Close()
	{
		fclose( mFile );
	}

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

	void Archive::Read( short* data, unsigned int length )
	{
		READ_DATA( data, short, length );
	}

	void Archive::Read( unsigned short* data, unsigned int length )
	{
		READ_DATA( data, unsigned short, length );
	}

	void Archive::Read( int* data, unsigned int length, bool is32bit )
	{
		if( is32bit )
		{
			READ_DATA( data, int, length );
		}
		else
		{
			for( unsigned int x = 0; x < length; ++x )
			{
				short d;
				READ_DATA( &d, short, 1 );
				data[ x ] = d;
			}
		}
	}

	void Archive::Read( unsigned int* data, unsigned int length, bool is32bit )
	{
		if( is32bit )
		{
			READ_DATA( data, unsigned int, length );
		}
		else
		{
			for( unsigned int x = 0; x < length; ++x )
			{
				unsigned short d;
				READ_DATA( &d, unsigned short, 1 );
				data[ x ] = d;
			}
		}
	}

	void Archive::Read( long* data, unsigned int length, bool is32bit )
	{
		if( is32bit )
		{
			READ_DATA( data, long, length );
		}
		else
		{
			for( unsigned long x = 0; x < length; ++x )
			{
				short d;
				READ_DATA( &d, short, 1 );
				data[ x ] = d;
			}
		}
	}

	void Archive::Read( unsigned long* data, unsigned int length, bool is32bit )
	{
		if( is32bit )
		{
			READ_DATA( data, unsigned long, length );
		}
		else
		{
			for( unsigned long x = 0; x < length; ++x )
			{
				unsigned short d;
				READ_DATA( &d, unsigned short, 1 );
				data[ x ] = d;
			}
		}
	}

	void Archive::Read( float* data, unsigned int length, bool is32bit )
	{
		if( is32bit )
		{
			READ_DATA( data, float, length );
		}
		else
		{
			for( unsigned int x = 0; x < length; ++x )
			{
				short d;
				READ_DATA( &d, short, 1 );
				data[ x ] = FloatCompressor::Decompress( d );
			}
		}
	}

	void Archive::Read( double* data, unsigned int length )
	{
		READ_DATA( data, double, length );
	}

	void Archive::Read( bool* data, unsigned int length )
	{
		for( unsigned int x = 0; x < length; ++x )
		{
			BYTE b;
			READ_DATA( &b, BYTE, 1 );
			data[ x ] = (b == 1) ? true : false;
		}
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

		data->shrink_to_fit();
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

	void Archive::Write( const short* data, unsigned int length )
	{
		WRITE_DATA( data, short, length );
	}

	void Archive::Write( const unsigned short* data, unsigned int length )
	{
		WRITE_DATA( data, unsigned short, length );
	}

	void Archive::Write( const int* data, unsigned int length, bool is32bit )
	{
		if( is32bit )
		{
			WRITE_DATA( data, int, length );
		}
		else
		{
			for( unsigned int x = 0; x < length; ++x )
			{
				short d = (short)data[ x ];
				WRITE_DATA( &d, short, 1 );
			}
		}
	}

	void Archive::Write( const unsigned int* data, unsigned int length, bool is32bit )
	{
		if( is32bit )
		{
			WRITE_DATA( data, unsigned int, length );
		}
		else
		{
			for( unsigned int x = 0; x < length; ++x )
			{
				unsigned short d = (unsigned short)data[ x ];
				WRITE_DATA( &d, unsigned short, 1 );
			}
		}
	}

	void Archive::Write( const long* data, unsigned int length, bool is32bit )
	{
		if( is32bit )
		{
			WRITE_DATA( data, long, length );
		}
		else
		{
			for( unsigned long x = 0; x < length; ++x )
			{
				short d = (short)data[ x ];
				WRITE_DATA( &d, short, 1 );
			}
		}
	}

	void Archive::Write( const unsigned long* data, unsigned int length, bool is32bit )
	{
		if( is32bit )
		{
			WRITE_DATA( data, unsigned long, length );
		}
		else
		{
			for( unsigned long x = 0; x < length; ++x )
			{
				unsigned short d = (unsigned short)data[ x ];
				WRITE_DATA( &d, unsigned short, 1 );
			}
		}
	}

	void Archive::Write( const float* data, unsigned int length, bool is32bit )
	{
		if( is32bit )
		{
			WRITE_DATA( data, float, length );
		}
		else
		{
			for( unsigned int x = 0; x < length; ++x )
			{
				short d = FloatCompressor::Compress( data[ x ] );
				WRITE_DATA( &d, short, 1 );
			}
		}
	}

	void Archive::Write( const double* data, unsigned int length )
	{
		WRITE_DATA( data, double, length );
	}

	void Archive::Write( const bool* data, unsigned int length )
	{
		for( unsigned int x = 0; x < length; ++x )
		{
			BYTE b = (data[ x ]) ? 1 : 0;
			WRITE_DATA( &b, BYTE, 1 );
		}
	}

	void Archive::Write( const std::string* data )
	{
		Write( data->c_str(), (unsigned int)data->size() );

		char end = 0;
		Write( &end );
	}

	/////////////////////////////////////////////////////////////

	unsigned int Archive::ToBuffer( const char *filename, char*& buf )
	{
		FILE* file = fopen( filename, "rb" );

		if( !file )
			return 0;

		fseek( file, 0, SEEK_END );
		unsigned int length = (unsigned int)( ftell( file ));
		rewind( file );

		buf = (char*)malloc( length + 1 );
		fread( buf, sizeof( char ), length, file );
		fclose( file );

		buf[ length ] = 0;

		return length;
	}
}
