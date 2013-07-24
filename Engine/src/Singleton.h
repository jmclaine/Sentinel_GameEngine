#pragma once

#include "CriticalSection.h"

namespace Sentinel
{
	template< class T >
	class Singleton
	{
	protected:

		static CriticalSection		mCS;

		static volatile T* volatile mSingle;

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
		
		//Singleton( const Singleton& );
		//void operator = ( const Singleton& );
	};

	//////////////////////////////////////////////////////////////////////////

	template< class T >
	class SingletonAbstract
	{
	protected:

		static CriticalSection		mCS;

		static volatile T* volatile mSingle;

	public:

		static T* volatile Inst( T*(*func)() )
		{
			if( !mSingle )
			{
				mCS.Lock();

				if( !mSingle )
					mSingle = func();

				mCS.Unlock();
			}
			
			return const_cast< T* >(mSingle);
		}

		static T* volatile Inst( T* obj )
		{
			if( !mSingle )
			{
				mCS.Lock();

				if( !mSingle )
					mSingle = (volatile T*)( obj );

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

	template< class T > volatile T* volatile Singleton< T >::mSingle = NULL;
	template< class T > CriticalSection Singleton< T >::mCS;

	template< class T > volatile T* volatile SingletonAbstract< T >::mSingle = NULL;
	template< class T > CriticalSection SingletonAbstract< T >::mCS;
}
