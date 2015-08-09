#pragma once

#include "Common.h"
#include "Util.h"

#include <list>
#include <functional>
#define DELEGATE( type ) std::function< type >
#define BIND( func ) std::bind( &func, this )

typedef DELEGATE( void() ) Action;

namespace Sentinel
{
	template< typename T >
	class Event
	{
	private:

		std::list< std::function< T >> events;

	public:

		Event()
		{}

		void operator += ( std::function< T > right )
		{
			events.push_back( right );
		}

		void operator -= ( std::function< T > right )
		{
			events.remove( right );
		}

		void Activate()
		{
			if( events.empty() )
				return;

			TRAVERSE_LIST( it, events )
				(*it)();
		}
	};
}
