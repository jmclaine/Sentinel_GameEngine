#pragma once

#include <stdio.h>

#include "Common.h"
#include "Vector2f.h"
#include "Vector3f.h"
#include "Matrix4f.h"

namespace Sentinel
{
	class SENTINEL_DLL FileIO
	{
	public:

		static bool	ToBuffer( const char *filename, char*& buf );

		static void	Read(  FILE* file, unsigned char& data );
		static void	Read(  FILE* file, int& data,					bool is32bit = false );
		static void	Read(  FILE* file, unsigned int& data,			bool is32bit = false );
		static void	Read(  FILE* file, float& data,					bool is32bit = false );
		static void	Read(  FILE* file, char* data, int length );
		static void	Read(  FILE* file, Vector2f& data,				bool is32bit = false );
		static void	Read(  FILE* file, Vector3f& data,				bool is32bit = false );
		static void Read(  FILE* file, Matrix4f& data,				bool is32bit = false );

		static void	Write( FILE* file, const unsigned char& data );
		static void	Write( FILE* file, const int& data,				bool is32bit = false );
		static void	Write( FILE* file, const unsigned int& data,	bool is32bit = false );
		static void	Write( FILE* file, const float& data,			bool is32bit = false );
		static void	Write( FILE* file, const char* str );
		static void	Write( FILE* file, const Vector2f& data,		bool is32bit = false );
		static void	Write( FILE* file, const Vector3f& data,		bool is32bit = false );
		static void Write( FILE* file, const Matrix4f& data,		bool is32bit = false );
	};
}
