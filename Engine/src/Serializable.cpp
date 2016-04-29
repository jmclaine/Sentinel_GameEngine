#include "Serializable.h"
#include "Archive.h"
#include "Util.h"

namespace Sentinel
{
	SerialFactory::SerialFactory()
	{
		Register(0, nullptrClone);
	}

	SerialFactory::~SerialFactory()
	{ }

	Serializable* SerialFactory::nullptrClone()
	{
		return nullptr;
	}

	SerialFactory& SerialFactory::Get()
	{
		static SerialFactory mSingle;
		return mSingle;
	}

	bool SerialFactory::Register(UINT value, CloneFunc func)
	{
		if (mRegistry.find(value) != mRegistry.end())
			return false;

		mRegistry.insert(std::pair<UINT, CloneFunc>(value, func));

		return true;
	}

	Serializable* SerialFactory::Create(UINT value)
	{
		auto it = mRegistry.find(value);

		if (it != mRegistry.end())
			return it->second();

		return nullptr;
	}

	////////////////////////////////////

	SerialRegister::SerialRegister(const char* clazz, SerialFactory::CloneFunc func)
	{
		mID = HashString(clazz);

		//Debug::Log(STREAM(clazz << ": " << mID));

		if (!SerialFactory::Get().Register(mID, func))
			throw AppException("Failed to register serial '" + std::string(clazz) + "'");
	}

	void SerialRegister::Save(Archive& archive)
	{
		archive.Write(&mID);
	}

	Serializable* SerialRegister::Load(Archive& archive)
	{
		UINT id;
		archive.Read(&id);

		if (id != 0)
		{
			//Debug::Log(STREAM(id));
			return SerialFactory::Get().Create(id);
		}

		return nullptr;
	}

	///////////////////////////////////////////////////////////////////////////////////

	void SerialFunctionFactory::nullptrFunc()
	{ }

	std::function< void() > SerialFunctionFactory::nullptrFuncPtr = std::function< void() >(nullptrFunc);

	SerialFunctionFactory::SerialFunctionFactory()
	{ }

	SerialFunctionFactory::~SerialFunctionFactory()
	{ }

	SerialFunctionFactory& SerialFunctionFactory::Get()
	{
		static SerialFunctionFactory mSingle;
		return mSingle;
	}

	///////////////////////////

	bool SerialFunctionFactory::Register(UINT value, SerialFunctionFactory::Func func)
	{
		if (mRegistry.find(value) != mRegistry.end())
			return false;

		mRegistry.insert(std::pair<UINT, SerialFunctionFactory::Func>(value, func));

		return true;
	}

	const std::function<void()>& SerialFunctionFactory::Create(UINT value)
	{
		auto it = mRegistry.find(value);

		if (it != mRegistry.end())
			return it->second;

		return nullptrFuncPtr;
	}

	UINT SerialFunctionFactory::Find(Func func)
	{
		if (func.target_type() == nullptrFuncPtr.target_type())
			return 0;

		TRAVERSE_LIST(it, mRegistry)
		{
			if (it->second.target_type() == func.target_type())
				return it->first;
		}

		return 0;
	}
}
