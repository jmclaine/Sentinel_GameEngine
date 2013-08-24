#pragma once

#include <unordered_map>

#include "CriticalSection.h"

namespace Sentinel
{
	// Singleton for the entire application.
	//
	template< class T >
	class Singleton
	{
	protected:

		static CriticalSection		mCS;

		static volatile T* volatile mSingle;

		/////////////////////////////////////////

		Singleton() {}
		Singleton( const Singleton& ) {}
		Singleton& operator = ( const Singleton& ) { return *this; }
		~Singleton() {}

	public:

		static T* volatile Inst()
		{
			if( !mSingle )
			{
				mCS.Lock();

				if( !mSingle )
					mSingle = new T();

				mCS.Unlock();
			}
			
			return const_cast< T* >(mSingle);
		}

		static void volatile Destroy()
		{
			if( mSingle )
			{
				mCS.Lock();

				delete mSingle;
				mSingle = NULL;

				mCS.Unlock();
			}
		}
	};

	template< class T > volatile T* volatile Singleton< T >::mSingle = NULL;
	template< class T > CriticalSection Singleton< T >::mCS;

	//////////////////////////////////////////////////////////////////////////

	// Singleton for the entire application for abstract classes.
	//
	template< class T >
	class SingletonAbstract
	{
	protected:

		static CriticalSection		mCS;

		static volatile T* volatile mSingle;

		/////////////////////////////////////////

		SingletonAbstract() {}
		SingletonAbstract( const SingletonAbstract& ) {}
		SingletonAbstract& operator = ( const SingletonAbstract& ) { return *this; }
		~SingletonAbstract() {}

	public:

		static T* volatile Inst( T* obj )
		{
			if( !mSingle )
			{
				mCS.Lock();

				if( !mSingle )
					mSingle = obj;

				mCS.Unlock();
			}
			
			return const_cast< T* >(mSingle);
		}

		static T* volatile Inst()
		{
			return const_cast< T* >(mSingle);
		}

		static void volatile Destroy()
		{
			if( mSingle )
			{
				mCS.Lock();

				delete mSingle;
				mSingle = NULL;

				mCS.Unlock();
			}
		}
	};

	template< class T > volatile T* volatile SingletonAbstract< T >::mSingle = NULL;
	template< class T > CriticalSection SingletonAbstract< T >::mCS;

	//////////////////////////////////////////////////////////////////////////

	// Singleton for a single thread.
	//
	template< class T >
	class SingletonThreaded
	{
	private:

		static std::unordered_map< DWORD, T* > mObj;

	protected:

		SingletonThreaded() {}
		SingletonThreaded( const SingletonThreaded& ) {}
		SingletonThreaded& operator = ( const SingletonThreaded& ) { return *this; }
		~SingletonThreaded() {}

	public:

		static T* Inst()
		{
			DWORD threadID = GetCurrentThreadId();

			if( mObj.find( threadID ) == mObj.end() )
				mObj[ threadID ] = new T();
			
			return mObj[ threadID ];
		}

		static void Destroy()
		{
			DWORD threadID = GetCurrentThreadId();

			if( mObj.find( threadID ) != mObj.end() )
			{
				delete mObj[ threadID ];
				mObj.erase( threadID );
			}
		}
	};

	template< class T > std::unordered_map< DWORD, T* > SingletonThreaded< T >::mObj;

	//////////////////////////////////////////////////////////////////////////

	// Singleton for a single thread for abstract classes.
	//
	template< class T >
	class SingletonAbstractThreaded
	{
	private:

		static std::unordered_map< DWORD, T* > mObj;

	protected:

		SingletonAbstractThreaded() {}
		SingletonAbstractThreaded( const SingletonAbstractThreaded& ) {}
		SingletonAbstractThreaded& operator = ( const SingletonAbstractThreaded& ) { return *this; }
		~SingletonAbstractThreaded() {}

	public:

		static T* Inst( T* obj )
		{
			DWORD threadID = GetCurrentThreadId();

			if( mObj.find( threadID ) == mObj.end() )
				mObj[ threadID ] = obj;
			
			return mObj[ threadID ];
		}

		static T* Inst()
		{
			return mObj[ GetCurrentThreadId() ];
		}

		static void Destroy()
		{
			DWORD threadID = GetCurrentThreadId();

			if( mObj.find( threadID ) != mObj.end() )
			{
				delete mObj[ threadID ];
				mObj.erase( threadID );
			}
		}
	};

	template< class T > std::unordered_map< DWORD, T* > SingletonAbstractThreaded< T >::mObj;

	//////////////////////////////////////////////////////////////////////////

	// Singleton for multiple windows.
	// The Inst(void*) makes this class useful for other OSs if necessary,
	// or for Singleton's based on any memory address rather than
	// requiring window handles specifically.
	//
	template< class T >
	class SingletonWindow
	{
	private:

		static std::unordered_map< void*, T* > mObj;

	protected:

		SingletonWindow() {}
		SingletonWindow( const SingletonWindow& ) {}
		SingletonWindow& operator = ( const SingletonWindow& ) { return *this; }
		~SingletonWindow() {}

	public:

		static T* Inst( T* obj, void* hWnd = NULL )
		{
			if( mObj.find( hWnd ) == mObj.end() )
				mObj[ hWnd ] = obj;
			
			return (T*)mObj[ hWnd ];
		}

		static T* Inst( void* hWnd = NULL )
		{
			if( mObj.find( hWnd ) == mObj.end() )
				mObj[ hWnd ] = new T();
			
			return (T*)mObj[ hWnd ];
		}

		static void Destroy( void* hWnd = NULL )
		{
			if( mObj.find( hWnd ) != mObj.end() )
			{
				delete mObj[ hWnd ];
				mObj.erase( hWnd );
			}
		}
	};

	template< class T > std::unordered_map< void*, T* > SingletonWindow< T >::mObj;

	//////////////////////////////////////////////////////////////////////////

	// Singleton for multiple windows on an abstract class.
	//
	template< class T >
	class SingletonAbstractWindow
	{
	private:

		static std::unordered_map< void*, T* > mObj;

	protected:

		SingletonAbstractWindow() {}
		SingletonAbstractWindow( const SingletonAbstractWindow& ) {}
		SingletonAbstractWindow& operator = ( const SingletonAbstractWindow& ) { return *this; }
		~SingletonAbstractWindow() {}

	public:

		static T* Inst( T* obj, void* hWnd = NULL )
		{
			if( mObj.find( hWnd ) == mObj.end() )
				mObj[ hWnd ] = obj;
			
			return (T*)mObj[ hWnd ];
		}

		static T* Inst( void* hWnd = NULL )
		{
			return (T*)mObj[ hWnd ];
		}

		static void Destroy( void* hWnd = NULL )
		{
			if( mObj.find( hWnd ) != mObj.end() )
			{
				delete mObj[ hWnd ];
				mObj.erase( hWnd );
			}
		}
	};

	template< class T > std::unordered_map< void*, T* > SingletonAbstractWindow< T >::mObj;
}
