#pragma once

#include <map>
#include <stdio.h>

#include "Common.h"

namespace Sentinel
{
	class Archive;

#define DECLARE_SERIAL()\
	private:\
		static SerialRegister mSerialRegistry;\
		static Serializeable* Clone();\
	public:\
		void Save( Archive& archive );\
		void Load( Archive& archive );

#define DEFINE_SERIAL_REGISTER_EX( refClass, cloneClass )\
	SerialRegister cloneClass::mSerialRegistry( #refClass, cloneClass::Clone );

#define DEFINE_SERIAL_REGISTER( clazz )\
	DEFINE_SERIAL_REGISTER_EX( clazz, clazz );

#define DEFINE_SERIAL_CLONE( clazz )\
	Serializeable* clazz::Clone() {\
		return new clazz(); }

#define DEFINE_SERIAL_SAVE( clazz )\
	void clazz::Save( Archive& archive ) {\
		mSerialRegistry.Save( archive );\
		archive.Write( (const char*)this, sizeof( clazz )); }

#define DEFINE_SERIAL_LOAD( clazz )\
	void clazz::Load( Archive& archive ) {\
		archive.Read( (char*)this, sizeof( clazz )); }

#define DEFINE_SERIAL( clazz )\
	DEFINE_SERIAL_REGISTER( clazz )\
	DEFINE_SERIAL_CLONE( clazz )\
	DEFINE_SERIAL_SAVE( clazz )\
	DEFINE_SERIAL_LOAD( clazz )


	// Inherit from this class.
	//
	// Clone() recreates the object as a Serializable object.
	//
	// Save() allows customization for output to a file
	// as it is relatively unknown how an object should be
	// exported. In some cases, pointers need specific
	// functions in order to be initialized correctly.
	//
	// Load() should be the inverse of Save()
	//
	class Serializeable
	{
	public:

		virtual ~Serializeable() {}

		virtual void	Save( Archive& archive ) = 0;
		
		virtual void	Load( Archive& archive ) = 0;
	};

	// Stores the Serializable->Clone() function of each object.
	// Singleton lazy instantiation.
	//
	class SENTINEL_DLL SerialFactory
	{
	public:

		typedef Serializeable* (*CloneFunc)();

	private:
		
		static Serializeable* NullClone();

		std::map< int, CloneFunc > mRegistry;

	protected:

		SerialFactory();

	public:

		~SerialFactory();

		static SerialFactory& Get();
		
		///////////////////////////

		void			Register( int value, CloneFunc func );

		Serializeable*	Create( int value );
	};

	// Automatically registers the class within the SerialFactory.
	//
	class SENTINEL_DLL SerialRegister
	{
	public:

		int mID;

		SerialRegister( const char* clazz, SerialFactory::CloneFunc func );

		void					Save( Archive& archive );

		static Serializeable*	Load( Archive& archive );
	};
}
