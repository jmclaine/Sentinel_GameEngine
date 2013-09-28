#include "Serializeable.h"

namespace Sentinel
{
	SerialFactory::SerialFactory()
	{}

	SerialFactory::~SerialFactory()
	{
		TRAVERSE_LIST( it, mRegistry )
			delete it->second;

		mRegistry.clear();
	}

	SerialFactory* SerialFactory::Inst()
	{
		static SerialFactory mSingle;
		return &mSingle;
	}

	void SerialFactory::Register( int value, Serializeable* obj )
	{
		mRegistry.insert( std::pair< int, Serializeable* >( value, obj ));
	}

	Serializeable* SerialFactory::Create( int value )
	{
		auto it = mRegistry.find( value );
		
		if( it != mRegistry.end() )
			return it->second->Clone();

		return NULL;
	}
}
