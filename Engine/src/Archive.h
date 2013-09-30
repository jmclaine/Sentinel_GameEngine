#pragma once

#include <stdio.h>

#include "Common.h"

namespace Sentinel
{
	class SENTINEL_DLL Archive
	{
	private:

		FILE* mFile;

	public:

		Archive( FILE* fp );
		~Archive();

		void	Read(  char* data,					unsigned int length = 1 );
		void	Read(  unsigned char* data,			unsigned int length = 1 );
		void	Read(  int* data, 					unsigned int length = 1, bool is32bit = false );
		void	Read(  unsigned int* data, 			unsigned int length = 1, bool is32bit = false );
		void	Read(  float* data,					unsigned int length = 1, bool is32bit = false );
		
		void	Write( const char* data,			unsigned int length = 1 );
		void	Write( const unsigned char* data,	unsigned int length = 1 );
		void	Write( const int* data, 			unsigned int length = 1, bool is32bit = false );
		void	Write( const unsigned int* data,	unsigned int length = 1, bool is32bit = false );
		void	Write( const float* data,			unsigned int length = 1, bool is32bit = false );

		////////////////////////

		static bool	ToBuffer( const char *filename, char*& buf );
	};
}
