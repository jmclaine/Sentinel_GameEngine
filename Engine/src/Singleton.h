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
		Singleton& operator = ( const Singleton& ) {}
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
		SingletonThreaded& operator = ( const SingletonThreaded& ) {}
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

		static T* volatile Inst( T* obj )
		{
			DWORD threadID = GetCurrentThreadId();

			if( mObj.find( threadID ) == mObj.end() )
				mObj[ threadID ] = obj;
			
			return mObj[ threadID ];
		}

		static T* volatile Inst()
		{
			return mObj[ GetCurrentThreadId() ];
		}

		static void volatile Destroy()
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
}
