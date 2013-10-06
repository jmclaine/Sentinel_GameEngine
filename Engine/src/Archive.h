#pragma once
/*
The flag is32bit refers to whether or not
the variable should be compressed to 16-bit
or remain as it is.

The default is to compress.

Note that it is lossy, but should be accurate
enough for most cases.

Also note that a double is never compressed.
*/
#include <stdio.h>

#include "Common.h"

namespace Sentinel
{
	class SENTINEL_DLL Archive
	{
	private:

		FILE* mFile;

	public:

		Archive();
		~Archive();
		
		bool	Open( const char* filename, const char* mode );
		void	Close();

		void	Read(  char* data,					unsigned int length = 1 );
		void	Read(  unsigned char* data,			unsigned int length = 1 );
		void	Read(  int* data, 					unsigned int length = 1, bool is32bit = false );
		void	Read(  unsigned int* data, 			unsigned int length = 1, bool is32bit = false );
		void	Read(  long* data, 					unsigned int length = 1, bool is32bit = false );
		void	Read(  unsigned long* data, 		unsigned int length = 1, bool is32bit = false );
		void	Read(  float* data,					unsigned int length = 1, bool is32bit = false );
		void	Read(  double* data,				unsigned int length = 1 );
		void	Read(  std::string* data );
		
		void	Write( const char* data,			unsigned int length = 1 );
		void	Write( const unsigned char* data,	unsigned int length = 1 );
		void	Write( const int* data, 			unsigned int length = 1, bool is32bit = false );
		void	Write( const unsigned int* data,	unsigned int length = 1, bool is32bit = false );
		void	Write( const long* data, 			unsigned int length = 1, bool is32bit = false );
		void	Write( const unsigned long* data, 	unsigned int length = 1, bool is32bit = false );
		void	Write( const float* data,			unsigned int length = 1, bool is32bit = false );
		void	Write( const double* data,			unsigned int length = 1 );
		void	Write( const std::string* data );

		////////////////////////

		// Returns the size of the allocation.
		//
		static unsigned int ToBuffer( const char *filename, char*& buf );
	};
}
