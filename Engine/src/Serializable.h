#pragma once

#include <map>
#include <stdio.h>
#include <functional>

#include "Sentinel.h"
#include "Hash.h"
#include "Exception.h"

namespace Sentinel
{
	class Archive;

#define DECLARE_SERIAL_ARCHIVE()\
	void Save(Archive& archive);\
	void Load(Archive& archive);

#define DECLARE_SERIAL()\
	private:\
		static SerialRegister SERIAL_REGISTER;\
		static Serializable* Clone();\
	public:\
		DECLARE_SERIAL_ARCHIVE();

#define DECLARE_SERIAL_REGISTER_SAVE()\
	void SerialSave(Archive& archive)

#define DEFINE_SERIAL_REGISTER_SAVE(clazz)\
	void clazz::SerialSave(Archive& archive) { SERIAL_REGISTER.Save(archive); Save(archive); }

#define DEFINE_SERIAL_REGISTER_EX(refClass, func)\
	SerialRegister refClass::SERIAL_REGISTER(#refClass, func);

#define DEFINE_SERIAL_REGISTER_CLONE(refClass, cloneClass)\
	SerialRegister cloneClass::SERIAL_REGISTER(#refClass, cloneClass::Clone);

#define DEFINE_SERIAL_REGISTER(clazz)\
	DEFINE_SERIAL_REGISTER_CLONE(clazz, clazz);

#define DEFINE_SERIAL_CLONE(clazz)\
	Serializable* clazz::Clone() { return new clazz(); }

#define DEFINE_SERIAL_CLONE_INLINE(clazz)\
	static Serializable* Clone() { return new clazz(); }

#define DEFINE_SERIAL_SAVE(clazz)\
	void clazz::Save(Archive& archive) {\
		SERIAL_REGISTER.Save(archive);\
		archive.Write((const char*)this, sizeof(clazz)); }

#define DEFINE_SERIAL_LOAD(clazz)\
	void clazz::Load(Archive& archive) {\
		archive.Read((char*)this, sizeof(clazz)); }

#define DEFINE_SERIAL(clazz)\
	DEFINE_SERIAL_REGISTER(clazz)\
	DEFINE_SERIAL_CLONE(clazz)\
	DEFINE_SERIAL_SAVE(clazz)\
	DEFINE_SERIAL_LOAD(clazz)


	// Inherit from this class.
	//
	// Clone() recreates the object as a Serializable object.
	//
	// Save() allows customization for output to a file
	// as it is relatively unknown how an object should be
	// exported. In some cases, pointers need specific
	// functions in order to be initialized correctly.
	//
	// Load() reads the data required for the object's
	// initialization.
	//
	class Serializable
	{
	public:
		virtual ~Serializable() { }

		virtual void Save(Archive& archive) = 0;
		virtual void Load(Archive& archive) = 0;
	};

	// Stores the Serializable->Clone() function of each object.
	// Singleton lazy instantiation.
	//
	class SENTINEL_DLL SerialFactory
	{
	public:
		typedef Serializable* (*CloneFunc)();

	private:
		static Serializable* nullptrClone();

		std::map<UINT, CloneFunc> mRegistry;

	protected:
		SerialFactory();

	public:
		~SerialFactory();

		static SerialFactory& Get();

		///////////////////////////

		bool Register(UINT value, CloneFunc func);

		Serializable* Create(UINT value);
	};

	// Automatically registers the class within the SerialFactory.
	//
	class SENTINEL_DLL SerialRegister
	{
	private:
		UINT mID;

	public:
		SerialRegister(const char* clazz, SerialFactory::CloneFunc func);

		void Save(Archive& archive);

		static Serializable* Load(Archive& archive);
	};

	///////////////////////////////////////////////////////////////////////////////////

#define REGISTER_SERIAL_FUNCTION(func)\
	SerialFunctionFactory::Get().Register(HashString(#func), std::function<void()>([=]() { func(); } ))

#define CREATE_SERIAL_FUNCTION(func)\
	SerialFunctionFactory::Get().Create(HashString(#func))

	class SENTINEL_DLL SerialFunctionFactory
	{
	public:
		typedef std::function<void()> Func;

	private:
		static void nullptrFunc();
		static std::function<void()> nullptrFuncPtr;

		std::map<UINT, Func> mRegistry;

	protected:
		SerialFunctionFactory();

	public:
		~SerialFunctionFactory();

		static SerialFunctionFactory& Get();

		///////////////////////////

		bool Register(UINT value, Func func);

		const std::function<void()>& Create(UINT value);

		UINT Find(Func func);
	};

	///////////////////////////////////////////////////////////////////////////////////

#define DECLARE_SERIAL_MEMBER_FUNCTION(clazz, func)\
	static SerialMemberFunctionRegister<clazz> SERIAL_##func;

#define DEFINE_SERIAL_MEMBER_FUNCTION(clazz, func)\
	SerialMemberFunctionRegister<clazz> clazz::SERIAL_##func(#func, &clazz::func);

	template<class Type>
	class SerialMemberFunctionFactory
	{
	public:
		typedef Serializable* (Type::*Func)();

	private:
		std::map<UINT, Func> mRegistry;

	protected:
		SerialMemberFunctionFactory()
		{ }

	public:
		~SerialMemberFunctionFactory()
		{ }

		static SerialMemberFunctionFactory& Get()
		{
			static SerialMemberFunctionFactory mSingle;
			return mSingle;
		}

		///////////////////////////

		bool Register(UINT value, Func func)
		{
			if (mRegistry.find(value) != mRegistry.end())
				return false;

			mRegistry.insert(std::pair<UINT, Func>(value, func));

			return true;
		}

		Func Create(UINT value)
		{
			auto it = mRegistry.find(value);

			if (it != mRegistry.end())
				return it->second;

			return 0;
		}

		UINT Find(Func func)
		{
			TRAVERSE_LIST(it, mRegistry)
			{
				if (it->second == func)
					return it->first;
			}

			return nullptr;
		}
	};

	// Automatically registers the class within the SerialMemberFunctionFactory.
	//
	template<class Type>
	class SerialMemberFunctionRegister
	{
	public:
		typedef Serializable* (Type::*Func)();

	private:
		UINT mID;

	public:
		SerialMemberFunctionRegister(const char* name, Func func)
		{
			mID = HashString(name);

			//Debug::Log(STREAM(name << ": " << mID));

			if (!SerialMemberFunctionFactory<Type>::Get().Register(mID, func))
				throw AppException("Failed to register serial '" + std::string(name) + "'");
		}

		void Save(Archive& archive)
		{
			archive.Write(&mID);
		}

		static Serializable* Load(Type* obj, Archive& archive)
		{
			UINT id;
			archive.Read(&id);

			if (id != 0)
			{
				//Debug::Log(STREAM(id));
				return (obj->*SerialMemberFunctionFactory<Type>::Get().Create(id))();
			}

			return nullptr;
		}
	};
}
