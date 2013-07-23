#pragma once

#include <memory>

namespace Sentinel
{
	template< class T >
	class Singleton
	{
	protected:

		static std::unique_ptr< T > single;

		Singleton()
		{
			single = NULL;
		}

	public:

		static T* Inst()
		{
			if( !single )
				single = std::unique_ptr< T >( new T );
			
			return single.get();
		}

		void Destroy()
		{
			if( single )
				single->Shutdown();
			
			single = NULL;
		}
		
		//Singleton( const Singleton& );
		//void operator = ( const Singleton& );
	};

	//////////////////////////////////////////////////////////////////////////

	template< class T >
	class SingletonAbstract
	{
	protected:

		static std::unique_ptr< T > single;

		SingletonAbstract()
		{
			single = NULL;
		}

	public:

		static T* Inst( T*(*func)() )
		{
			if( !single )
				single = std::unique_ptr< T >( func() );
			
			return single.get();
		}

		static T* Inst( T* obj )
		{
			if( !single )
				single = std::unique_ptr< T >( obj );
			
			return single.get();
		}

		static T* Inst()
		{
			return single.get();
		}

		void Destroy()
		{
			if( single )
				single->Shutdown();
			
			single = NULL;
		}
	};

	template< class T > std::unique_ptr< T > Singleton< T >::single				= NULL;
	template< class T > std::unique_ptr< T > SingletonAbstract< T >::single		= NULL;
}
