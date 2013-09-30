#include "Serializeable.h"
#include "Util.h"

namespace Sentinel
{
	SerialFactory::SerialFactory()
	{
		Register( 0, NullClone );
	}

	SerialFactory::~SerialFactory()
	{
		mRegistry.clear();
	}

	Serializeable* SerialFactory::NullClone()
	{
		return NULL;
	}

	SerialFactory* SerialFactory::Inst()
	{
		static SerialFactory mSingle;
		return &mSingle;
	}

	void SerialFactory::Register( int value, CloneFunc func )
	{
		mRegistry.insert( std::pair< int, CloneFunc >( value, func ));
	}

	Serializeable* SerialFactory::Create( int value )
	{
		auto it = mRegistry.find( value );
		
		if( it != mRegistry.end() )
			return it->second();

		return NULL;
	}

	////////////////////////////////////

	SerialRegister::SerialRegister( const char* clazz, SerialFactory::CloneFunc func )
	{
		mID = StringToID( clazz );
		SerialFactory::Inst()->Register( mID, func );
	}

	void SerialRegister::Save( Archive& archive )
	{
		archive.Write( &mID, 1, true );
	}

	Serializeable* SerialRegister::Load( Archive& archive )
	{
		int id;
		archive.Read( &id, 1, true );
		Serializeable* obj = SerialFactory::Inst()->Create( id );
		obj->Load( archive );
		return obj;
	}
}
