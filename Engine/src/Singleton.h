#pragma once

#include <unordered_map>

#include "CriticalSection.h"

namespace Sentinel
{
	// Singleton for the entire application.
	//
	template<class Type>
	class Singleton
	{
	protected:

		static Type* mSingle;

		/////////////////////////////////////////

		Singleton() {}
		Singleton(const Singleton&) {}
		Singleton& operator = (const Singleton&) { return *this; }
		~Singleton() {}

	public:

		static Type* volatile Inst()
		{
			if (!mSingle)
				mSingle = new Type();

			return const_cast<Type*>(mSingle);
		}

		static Type* volatile Inst(Type* obj)
		{
			if (!mSingle)
				mSingle = obj;

			return const_cast<Type*>(mSingle);
		}

		static void volatile Destroy()
		{
			if (mSingle)
			{
				delete mSingle;
				mSingle = NULL;
			}
		}
	};

	template<class Type> Type* Singleton<Type>::mSingle = NULL;

	//////////////////////////////////////////////////////////////////////////

	// Singleton for the entire application for abstract classes.
	//
	template<class Type>
	class SingletonAbstract
	{
	protected:

		static Type* mSingle;

		/////////////////////////////////////////

		SingletonAbstract() {}
		SingletonAbstract(const SingletonAbstract&) {}
		SingletonAbstract& operator = (const SingletonAbstract&) { return *this; }
		~SingletonAbstract() {}

	public:

		static Type* Inst(Type* obj)
		{
			if (!mSingle)
				mSingle = obj;

			return const_cast<Type*>(mSingle);
		}

		static Type* Inst()
		{
			return const_cast<Type*>(mSingle);
		}

		static void Destroy()
		{
			if (mSingle)
			{
				delete mSingle;
				mSingle = NULL;
			}
		}
	};

	template<class Type> Type* SingletonAbstract<Type>::mSingle = NULL;

	//////////////////////////////////////////////////////////////////////////

	// Singleton for the entire application.
	//
	template<class Type>
	class SingletonSafe
	{
	protected:

		static CriticalSection mCS;

		static volatile Type* volatile mSingle;

		/////////////////////////////////////////

		SingletonSafe() {}
		SingletonSafe(const SingletonSafe&) {}
		SingletonSafe& operator = (const SingletonSafe&) { return *this; }
		~SingletonSafe() {}

	public:

		static Type* volatile Inst()
		{
			if (!mSingle)
			{
				mCS.Lock();

				if (!mSingle)
					mSingle = new Type();

				mCS.Unlock();
			}

			return const_cast<Type*>(mSingle);
		}

		static Type* volatile Inst(Type* obj)
		{
			if (!mSingle)
			{
				mCS.Lock();

				if (!mSingle)
					mSingle = obj;

				mCS.Unlock();
			}

			return const_cast<T*>(mSingle);
		}

		static void volatile Destroy()
		{
			if (mSingle)
			{
				mCS.Lock();

				delete mSingle;
				mSingle = NULL;

				mCS.Unlock();
			}
		}
	};

	template<class Type> volatile Type* volatile SingletonSafe<Type>::mSingle = NULL;
	template<class Type> CriticalSection SingletonSafe<Type>::mCS;

	//////////////////////////////////////////////////////////////////////////

	// Singleton for the entire application for abstract classes.
	//
	template<class Type>
	class SingletonAbstractSafe
	{
	protected:

		static CriticalSection mCS;

		static volatile T* volatile mSingle;

		/////////////////////////////////////////

		SingletonAbstractSafe() {}
		SingletonAbstractSafe(const SingletonAbstractSafe&) {}
		SingletonAbstractSafe& operator = (const SingletonAbstractSafe&) { return *this; }
		~SingletonAbstractSafe() {}

	public:

		static Type* volatile Inst(Type* obj)
		{
			if (!mSingle)
			{
				mCS.Lock();

				if (!mSingle)
					mSingle = obj;

				mCS.Unlock();
			}

			return const_cast<Type*>(mSingle);
		}

		static Type* volatile Inst()
		{
			return const_cast<Type*>(mSingle);
		}

		static void volatile Destroy()
		{
			if (mSingle)
			{
				mCS.Lock();

				delete mSingle;
				mSingle = NULL;

				mCS.Unlock();
			}
		}
	};

	template<class Type> volatile Type* volatile SingletonAbstractSafe<Type>::mSingle = NULL;
	template<class Type> CriticalSection SingletonAbstractSafe<Type>::mCS;

	//////////////////////////////////////////////////////////////////////////

	// Singleton for a single thread.
	//
	template<class Type>
	class SingletonThreaded
	{
	private:

		static std::unordered_map<DWORD, Type*> mObj;

	protected:

		SingletonThreaded() {}
		SingletonThreaded(const SingletonThreaded&) {}
		SingletonThreaded& operator = (const SingletonThreaded&) { return *this; }
		~SingletonThreaded() {}

	public:

		static Type* Inst()
		{
			DWORD threadID = GetCurrentThreadId();

			if (mObj.find(threadID) == mObj.end())
				mObj[threadID] = new Type();

			return mObj[threadID];
		}

		static void Destroy()
		{
			DWORD threadID = GetCurrentThreadId();

			if (mObj.find(threadID) != mObj.end())
			{
				delete mObj[threadID];
				mObj.erase(threadID);
			}
		}
	};

	template<class Type> std::unordered_map<DWORD, Type*> SingletonThreaded<Type>::mObj;

	//////////////////////////////////////////////////////////////////////////

	// Singleton for a single thread for abstract classes.
	//
	template<class Type>
	class SingletonAbstractThreaded
	{
	private:

		static std::unordered_map<DWORD, Type*> mObj;

	protected:

		SingletonAbstractThreaded() {}
		SingletonAbstractThreaded(const SingletonAbstractThreaded&) {}
		SingletonAbstractThreaded& operator = (const SingletonAbstractThreaded&) { return *this; }
		~SingletonAbstractThreaded() {}

	public:

		static Type* Inst(Type* obj)
		{
			DWORD threadID = GetCurrentThreadId();

			if (mObj.find(threadID) == mObj.end())
				mObj[threadID] = obj;

			return mObj[threadID];
		}

		static Type* Inst()
		{
			return mObj[GetCurrentThreadId()];
		}

		static void Destroy()
		{
			DWORD threadID = GetCurrentThreadId();

			if (mObj.find(threadID) != mObj.end())
			{
				delete mObj[threadID];
				mObj.erase(threadID);
			}
		}
	};

	template<class Type> std::unordered_map<DWORD, Type*> SingletonAbstractThreaded<Type>::mObj;
}
