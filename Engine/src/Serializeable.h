#pragma once

#include <map>
#include <stdio.h>

#include "Util.h"

namespace Sentinel
{

#define DECLARE_SERIAL( clazz )\
	static SerialObject mSerialRegistry;\
	Serializeable*	Clone() const;\
	void Serialize( FILE* file );\
	void Deserialize( FILE* file );

#define DEFINE_SERIAL_OBJ( clazz )\
	SerialObject clazz::mSerialRegistry( #clazz, new clazz() );

#define DEFINE_SERIAL_CLONE( clazz )\
	Serializeable* clazz::Clone() const { return new clazz(); }

#define DEFINE_SERIALIZE( clazz )\
	void clazz::Serialize( FILE* file ) { FileIO::Write( file, (const char*)this, sizeof( clazz )); }

#define DEFINE_DESERIALIZE( clazz )\
	void clazz::Deserialize( FILE* file ) { FileIO::Read( file, (char*)this, sizeof( clazz )); }

#define DEFINE_SERIAL( clazz )\
	DEFINE_SERIAL_OBJ( clazz )\
	DEFINE_SERIAL_CLONE( clazz )\
	DEFINE_SERIALIZE( clazz )\
	DEFINE_DESERIALIZE( clazz )

	// Inherit from this class.
	//
	// Clone() recreates the object as a Serializable object.
	//
	// Serialize() allows customization for output to a file
	// as it is relatively unknown how an object should be
	// exported. In some cases, pointers need specific
	// functions in order to be initialized correctly.
	//
	// Deserialize() should be the inverse of Serialize()
	//
	class Serializeable
	{
	public:

		virtual ~Serializeable() {}

		virtual Serializeable*	Clone() const = 0;

		virtual void			Serialize( FILE* file ) = 0;
		
		virtual void			Deserialize( FILE* file ) = 0;
	};

	// A simple singleton implementation.
	//
	// Stores the Serializable objects for Cloning.
	//
	class SerialFactory
	{
	private:

		std::map< int, Serializeable* >		mRegistry;

	protected:

		SerialFactory();

	public:

		~SerialFactory();

		static SerialFactory* Inst();
		
		///////////////////////////

		void			Register( int value, Serializeable* obj );

		Serializeable*	Create( int value );
	};

	// Automatically registers the class within the SerialFactory.
	//
	class SerialObject
	{
	public:

		SerialObject( const std::string& clazz, Serializeable* obj )
		{
			// Convert the class into a likely unique ID.
			//
			SerialFactory::Inst()->Register( StringToID( clazz ), obj );
		}
	};
}
