#include "Serializable.h"
#include "Util.h"
#include "Archive.h"

namespace Sentinel
{
	SerialFactory::SerialFactory()
	{
		Register( 0, NullClone );
	}

	SerialFactory::~SerialFactory()
	{}

	Serializable* SerialFactory::NullClone()
	{
		return NULL;
	}

	SerialFactory& SerialFactory::Get()
	{
		static SerialFactory mSingle;
		return mSingle;
	}

	bool SerialFactory::Register( UINT value, CloneFunc func )
	{
		if( mRegistry.find( value ) != mRegistry.end() )
			return false;

		mRegistry.insert( std::pair< UINT, CloneFunc >( value, func ));

		return true;
	}

	Serializable* SerialFactory::Create( UINT value )
	{
		auto it = mRegistry.find( value );
		
		if( it != mRegistry.end() )
			return it->second();

		return NULL;
	}

	////////////////////////////////////

	SerialRegister::SerialRegister( const char* clazz, SerialFactory::CloneFunc func )
	{
		mID = HashString( clazz );
		if( !SerialFactory::Get().Register( mID, func ))
			throw AppException( "Failed to register serial '" + std::string( clazz ) + "'" );
	}

	void SerialRegister::Save( Archive& archive )
	{
		archive.Write( &mID, 1, true );
	}

	Serializable* SerialRegister::Load( Archive& archive )
	{
		UINT id;
		archive.Read( &id, 1, true );

		if( id != 0 )
			return SerialFactory::Get().Create( id );

		return NULL;
	}

	///////////////////////////////////////////////////////////////////////////////////

	void SerialFunctionFactory::NullFunc()
	{}

	std::function< void() > SerialFunctionFactory::NullFuncPtr = std::function< void() >( NullFunc );

	SerialFunctionFactory::SerialFunctionFactory()
	{}

	SerialFunctionFactory::~SerialFunctionFactory()
	{}

	SerialFunctionFactory& SerialFunctionFactory::Get()
	{
		static SerialFunctionFactory mSingle;
		return mSingle;
	}
		
	///////////////////////////

	bool SerialFunctionFactory::Register( UINT value, SerialFunctionFactory::Func func )
	{
		if( mRegistry.find( value ) != mRegistry.end() )
			return false;

		mRegistry.insert( std::pair< UINT, SerialFunctionFactory::Func >( value, func ));

		return true;
	}

	const std::function< void() >& SerialFunctionFactory::Create( UINT value )
	{
		auto it = mRegistry.find( value );

		if( it != mRegistry.end() )
			return it->second;

		return NullFuncPtr;
	}

	UINT SerialFunctionFactory::Find( Func func )
	{
		if( func.target_type() == NullFuncPtr.target_type() )
			return 0;

		TRAVERSE_LIST( it, mRegistry )
		{
			if( it->second.target_type() == func.target_type() )
				return it->first;
		}

		return 0;
	}
}
