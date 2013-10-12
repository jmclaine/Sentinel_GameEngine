#pragma once
/*
The flag is32bit refers to whether or not
the variable should be compressed to 16-bit
or remain as it is.

The compression is lossy for floats.
All int based values are truncated to shorts.

A double is never compressed.
*/
#include <stdio.h>

#include "Common.h"

namespace Sentinel
{
	template< typename T >
	unsigned int ar_sizeof( T& var )
	{
		return sizeof( var ) / sizeof( *(var.Ptr()) );
	}

	class SENTINEL_DLL Archive
	{
	public:

		FILE*	mFile;

		////////////////////////

		Archive();
		~Archive();
		
		bool	Open( const char* filename, const char* mode );
		void	Close();

		void	Read(  char* data,					unsigned int length = 1 );
		void	Read(  unsigned char* data,			unsigned int length = 1 );
		void	Read(  short* data,					unsigned int length = 1 );
		void	Read(  unsigned short* data,		unsigned int length = 1 );
		void	Read(  int* data, 					unsigned int length = 1, bool is32bit = true );
		void	Read(  unsigned int* data, 			unsigned int length = 1, bool is32bit = true );
		void	Read(  long* data, 					unsigned int length = 1, bool is32bit = true );
		void	Read(  unsigned long* data, 		unsigned int length = 1, bool is32bit = true );
		void	Read(  float* data,					unsigned int length = 1, bool is32bit = true );
		void	Read(  double* data,				unsigned int length = 1 );
		void	Read(  std::string* data );
		
		void	Write( const char* data,			unsigned int length = 1 );
		void	Write( const unsigned char* data,	unsigned int length = 1 );
		void	Write( short* data,					unsigned int length = 1 );
		void	Write( unsigned short* data,		unsigned int length = 1 );
		void	Write( const int* data, 			unsigned int length = 1, bool is32bit = true );
		void	Write( const unsigned int* data,	unsigned int length = 1, bool is32bit = true );
		void	Write( const long* data, 			unsigned int length = 1, bool is32bit = true );
		void	Write( const unsigned long* data, 	unsigned int length = 1, bool is32bit = true );
		void	Write( const float* data,			unsigned int length = 1, bool is32bit = true );
		void	Write( const double* data,			unsigned int length = 1 );
		void	Write( const std::string* data );

		////////////////////////

		// Returns the size of the allocation.
		//
		static unsigned int ToBuffer( const char *filename, char*& buf );
	};
}
