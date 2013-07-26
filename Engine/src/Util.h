#pragma once

#include "Common.h"

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>
#include <algorithm>

#ifndef WIN32
    #define OutputDebugStringA( str )\
        printf( "%s\n", str );

    #define MessageBoxA( ptr, msg, title, flags )\
        printf( "%s: %s\n", title, msg );

    #define strcat_s    strcat
    #define sprintf_s   sprintf

	typedef unsigned int   DWORD;
	typedef unsigned short WORD;
	typedef unsigned char  UCHAR;
	typedef unsigned char  BYTE;
#else
	#include <windows.h>
#endif

#include "Vector2f.h"
#include "Vector3f.h"
#include "Vector4f.h"
#include "Color.h"

namespace Sentinel
{
	class InitFailed {};

	#define MAX_VALUE(a,b) (((a) > (b)) ? (a) : (b))
	#define MIN_VALUE(a,b) (((a) < (b)) ? (a) : (b))

	#define RV( func ) if( !func ) return 0;
	#define TV( func ) if( !func ) throw InitFailed();

	#define UNUSED( var ) (void)var;

	#define TRACE_NE( text )\
	{\
		std::stringstream str;\
		str << text;\
		OutputDebugStringA( str.str().c_str() );\
		std::cout << str.str().c_str();\
	}
	#define TRACE( text )	TRACE_NE( text << std::endl )

	#define REPORT_ERROR( err_msg, title )\
	{\
		std::stringstream str1, str2;\
		str1 << err_msg << std::endl;\
		str2 << title << std::endl;\
		MessageBoxA( NULL, str1.str().c_str(), str2.str().c_str(), MB_OK | MB_ICONERROR);\
	}

	#define TRAVERSE_VECTOR( x, vec ) for( UINT x = 0; x < vec.size(); ++x )

	#define TRAVERSE_LIST( it, obj ) for( auto it = obj.begin(); it != obj.end(); ++it )

	#define TRAVERSE_LIST_OFFSET( it, obj, offset ) for( auto it = obj.begin()+offset; it != obj.end(); ++it )

	#define COPY_ARRAY( dest, src, size )\
		for( UINT x = 0; x < size; ++x )\
			dest[ x ] = src[ x ];

	#define SAFE_DELETE( var )\
		if( var )\
		{\
			delete (var);\
			(var) = NULL;\
		}

	#define SAFE_DELETE_ARRAY( var )\
		if( var )\
		{\
			delete[] (var);\
			(var) = NULL;\
		}

	#define SAFE_RELEASE_DELETE( var )\
		if( var )\
		{\
			var->Release();\
			delete (var);\
			(var) = NULL;\
		}

	#define SAFE_RELEASE_PTR( var )\
		if( var ) { (var)->Release(); (var) = NULL; }

	#define SAFE_RELEASE_PTR_LIST( var )\
		TRAVERSE_LIST( it, var )\
			SAFE_RELEASE_PTR( (*it) );

	#define SAFE_RELEASE_LIST( var )\
		TRAVERSE_LIST( it, var )\
			(*it).Release();\
		(*it).clear();

	#define SHUTDOWN_ARRAY( count, var )\
		for( int x = 0; x < count; ++x )\
		{\
			var[ x ].Shutdown();\
		}\
		if( count > 0 )\
		{\
			delete[] var;\
		}

	#define OUTPUT_ARRAY( count, var )\
		for( int x = 0; x < count; ++x )\
		{\
			var[ x ].Output( file, width + 1 );\
		}

	#define INTERP( start, end, alpha ) ((start) + ((end) - (start)) * (alpha))

	////////////////////////////////////////////////////////////////////

	class AppException : public std::exception
	{
		const char* mMessage;

	public:

		AppException( const std::string& msg )
		{
			mMessage = msg.c_str();
		}

		const char* what() const throw()
		{
			return mMessage;
		}
	};

	////////////////////////////////////////////////////////////////////

	SENTINEL_DLL void		SetDirectory( const char* dest );

	SENTINEL_DLL bool		FileToBuffer( const char *filename, char*& buf );

	SENTINEL_DLL BYTE		ReadByte(   std::ifstream& file );
	SENTINEL_DLL int		ReadInt(    std::ifstream& file, bool is32bit = false );
	SENTINEL_DLL float		ReadFloat(  std::ifstream& file, bool is32bit = false );
	SENTINEL_DLL void		ReadString( std::ifstream& file, char* str, int length );
	SENTINEL_DLL Vector2f	ReadPoint2( std::ifstream& file, bool is32bit = false );
	SENTINEL_DLL Vector3f	ReadPoint3( std::ifstream& file, bool is32bit = false );

	template< typename Real >
	inline void CopyArray( Real& dest, const Real& src, UINT size )
	{
		for( UINT x = 0; x < size; ++x )
		{
			dest[ x ] = src[ x ];
		}
	}

	template< class ForwardIterator, class T >
	ForwardIterator binary_find( ForwardIterator begin, ForwardIterator end, T val )
	{
		ForwardIterator i = std::lower_bound( begin, end, val );

		return (i != end && *i == val) ? i : end;
	}

	////////////////////////////////////////////////////////////////////

	inline int RandomValue( int minValue, int maxValue )
	{
		return minValue + rand()%(maxValue - minValue);
	}

	inline float RandomValue( float minValue, float maxValue )
	{
		return minValue + (maxValue - minValue) * (rand() / static_cast< float >( RAND_MAX ));
	}

	inline double RandomValue( double minValue, double maxValue )
	{
		return minValue + (maxValue - minValue) * (rand() / static_cast< double >( RAND_MAX ));
	}

	inline Vector3f RandomValue( const Vector3f& minValue, const Vector3f& maxValue )
	{
		return Vector3f( RandomValue( minValue.X(), maxValue.X() ), \
						 RandomValue( minValue.Y(), maxValue.Y() ), \
						 RandomValue( minValue.Z(), maxValue.Z() ));
	}

	inline ColorRGBA RandomValue( const ColorRGBA& minColor, const ColorRGBA& maxColor )
	{
		return ColorRGBA( RandomValue( minColor.R(), maxColor.R() ), \
						  RandomValue( minColor.G(), maxColor.G() ), \
						  RandomValue( minColor.B(), maxColor.B() ), \
						  RandomValue( minColor.A(), maxColor.A() ));
	}

	////////////////////////////////////////////////////////////////////

	template< typename Real >
	inline Real clamp( Real value, Real minValue, Real maxValue )
	{
		return value < minValue ? minValue : value > maxValue ? maxValue : value;
	}

	template< typename Real >
	inline Real wrap( Real value, Real minValue, Real maxValue )
	{
		return value < minValue ? maxValue : value > maxValue ? minValue : value;
	}

	template< typename Real >
	inline Real wrapNP( Real value, Real minValue, Real maxValue )
	{
		return value < minValue ? value + maxValue : value > maxValue ? value + minValue : value;
	}

	template< typename T, typename AlphaT >
	inline T lerp( const T& start, const T& end, AlphaT alpha )
	{
		return start + end.Sub( start ) * alpha;
	}

	inline bool IsPowerOfTwo( UINT x )
	{
		return (x != 0) && ((x & (x-1)) == 0);
	}

	inline int FastMod2( int num )
	{
		return num & 1;
	}

	template< typename Real >
	void NUMtoBYTES( UCHAR* data, Real value )
	{
		unsigned result = *(reinterpret_cast< unsigned* >( &value ));

		for( unsigned i = 0; i < 4; ++i )
		{
			data[ i ] = (result & 0xFF);
			result >>= 8;
		}
	}

	template< typename Real >
	inline void BYTEStoNUM( UCHAR* data, Real& type )
	{
		unsigned result = 0;

		for( unsigned i = 0; i < 4; ++i )
		{
			result = (result << 8);
			result = (result | data[ 3 - i ]);
		}

		type = *(reinterpret_cast< Real* >( &result ));
	}

	////////////////////////////////////////////////////////////////////

	class RefCounter
	{
		int refc;

	public:

		RefCounter() :
			refc( 0 )
		{}

		void incRef()
		{
			refc++;
		}

		int decRef()
		{
			return --refc;
		}
	};

	template< typename T >
	class RefObject : public RefCounter
	{
	public:

		T data;

		RefObject()
		{
			incRef();
		}

		RefObject( T _data )
		{
			data = _data;
			incRef();
		}

		void assign( const T& right )
		{
			incRef();
			data = right;
		}

		operator T () { return data; }
		T& operator -> () { return data; }
	};

	////////////////////////////////////////////////////////////////////
	// Floating point compressor.
	// Provided by Phernost @ stackoverflow.com
	//
	class FloatCompressor
	{
		union Bits
		{
			float f;
			int32_t si;
			int32_t ui;
		};

		static int const shift = 13;
		static int const shiftSign = 16;

		static int32_t const infN = 0x7F800000;		// flt32 infinity
		static int32_t const maxN = 0x477FE000;		// max flt16 normal as a flt32
		static int32_t const minN = 0x38800000;		// min flt16 normal as a flt32
		static int32_t const signN = 0x80000000;	// flt32 sign bit

		static int32_t const infC = infN >> shift;
		static int32_t const nanN = (infC + 1) << shift; // minimum flt16 nan as a flt32
		static int32_t const maxC = maxN >> shift;
		static int32_t const minC = minN >> shift;
		static int32_t const signC = signN >> shiftSign; // flt16 sign bit

		static int32_t const mulN = 0x52000000;		// (1 << 23) / minN
		static int32_t const mulC = 0x33800000;		// minN / (1 << (23 - shift))

		static int32_t const subC = 0x003FF;		// max flt32 subnormal down shifted
		static int32_t const norC = 0x00400;		// min flt32 normal down shifted

		static int32_t const maxD = infC - maxC - 1;
		static int32_t const minD = minC - subC - 1;

	public:

		static int16_t Compress( float value )
		{
			Bits v, s;
			v.f = value;
			int32_t sign = v.si & signN;
			v.si ^= sign;
			sign >>= shiftSign; // logical shift
			s.si = mulN;
			s.si = (int)(s.f * v.f); // correct subnormals
			v.si ^= (s.si ^ v.si) & -(minN > v.si);
			v.si ^= (infN ^ v.si) & -((infN > v.si) & (v.si > maxN));
			v.si ^= (nanN ^ v.si) & -((nanN > v.si) & (v.si > infN));
			v.ui >>= shift; // logical shift
			v.si ^= ((v.si - maxD) ^ v.si) & -(v.si > maxC);
			v.si ^= ((v.si - minD) ^ v.si) & -(v.si > subC);
			return (int16_t)(v.ui | sign);
		}

		static float Decompress( int16_t value )
		{
			Bits v;
			v.ui = value;
			int32_t sign = v.si & signC;
			v.si ^= sign;
			sign <<= shiftSign;
			v.si ^= ((v.si + minD) ^ v.si) & -(v.si > subC);
			v.si ^= ((v.si + maxD) ^ v.si) & -(v.si > maxC);
			Bits s;
			s.si = mulC;
			s.f *= v.si;
			int32_t mask = -(norC > v.si);
			v.si <<= shift;
			v.si ^= (s.si ^ v.si) & mask;
			v.si |= sign;
			return v.f;
		}
	};
}
