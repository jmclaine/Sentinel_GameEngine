#pragma once

#include "Common.h"

#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <algorithm>

#ifndef WIN32
    #define OutputDebugStringA( str )\
        printf( "%s\n", str );

    #define MessageBoxA( ptr, msg, title, flags )\
        printf( "%s: %s\n", title, msg );

    #define strcat_s    strcat
    #define sprintf_s   sprintf
#else
	#include <windows.h>
#endif

namespace Sentinel
{
	class InitFailed {};

	typedef void( *FuncPtr0 )();

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

	#define TRAVERSE_VECTOR( x, vec ) for( unsigned int x = 0; x < vec.size(); ++x )

	#define TRAVERSE_LIST( it, obj ) for( auto it = obj.begin(); it != obj.end(); ++it )

	#define TRAVERSE_LIST_OFFSET( it, obj, offset ) for( auto it = obj.begin()+offset; it != obj.end(); ++it )

	#define COPY_ARRAY( dest, src, size )\
		for( unsigned int x = 0; x < size; ++x )\
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
			SAFE_RELEASE_PTR( (*it) );\
		var.clear();

	#define SAFE_RELEASE_LIST( var )\
		TRAVERSE_LIST( it, var )\
			(*it).Release();\
		(*it).clear();

	#define SHUTDOWN_DELETE( var )\
		if( var ) {\
			var->Shutdown();\
			delete var;\
			var = NULL; }

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

	template< class T >
	std::shared_ptr< T > SHARED( T* ptr )
	{
		return std::shared_ptr< T >(ptr);
	}

	// djb2 algorithm
	// Warning: Collisions may be possible
	//
	UINT SENTINEL_DLL HashString( const char* str );
	
	////////////////////////////////////////////////////////////////////

	class AppException : public std::exception
	{
		std::string mMessage;

	public:

		AppException( const std::string& msg )
		{
			mMessage = msg;
		}

		const char* what() const throw()
		{
			TRACE( mMessage );
			
			return mMessage.c_str();
		}
	};

	////////////////////////////////////////////////////////////////////

	SENTINEL_DLL void SetDirectory( const char* dest );
	SENTINEL_DLL std::string GetDirectory();

	template< typename Real >
	inline void CopyArray( Real& dest, const Real& src, UINT size )
	{
		for( UINT x = 0; x < size; ++x )
			dest[ x ] = src[ x ];
	}

	template< class ForwardIterator, class T >
	ForwardIterator binary_find( ForwardIterator begin, ForwardIterator end, T val )
	{
		ForwardIterator i = std::lower_bound( begin, end, val );

		return (i != end && *i == val) ? i : end;
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
		return start + (end - start) * alpha;
	}

	inline bool IsPowerOf2( UINT x )
	{
		return (x != 0) && ((x & (x-1)) == 0);
	}

	inline int FastMod2( int num )
	{
		return num & 1;
	}

	inline UINT NextPowerOf2( UINT n )
	{
		--n;

		n = n | (n >> 1);
        n = n | (n >> 2);
        n = n | (n >> 4);
        n = n | (n >> 8);
        n = n | (n >> 16);

		return n + 1;
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
}
