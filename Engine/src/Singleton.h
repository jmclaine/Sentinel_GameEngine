#pragma once

#include <memory>

namespace Sentinel
{
	template< class T >
	class Singleton
	{
	public:

		static std::unique_ptr< T > single;

		Singleton()
		{
			single = NULL;
		}

	public:

		static T* Inst()
		{
			if( !single )
			{
				single = std::unique_ptr< T >( new T );
			}

			return single.get();
		}

		void Destroy()
		{
			if( single )
			{
				single->release();
			}

			single = NULL;
		}
		
		Singleton( const Singleton& );
		void operator = ( const Singleton& );
	};

	//////////////////////////////////////////////////////////////////////////

	template< class T >
	class SingletonAbstract : public Singleton< T >
	{
	protected:

		SingletonAbstract()
		{}

	public:

		static T* Inst( T*(*func)() )
		{
			if( !single )
			{
				single = std::unique_ptr< T >( func() );
			}

			return single.get();
		}

		static T* Inst()
		{
			return single.get();
		}
	};

	template< class T > std::unique_ptr< T > Singleton< T >::single;
}
