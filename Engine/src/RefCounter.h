#pragma once

namespace Sentinel
{
	class RefCounter
	{
		int refc;

	public:

		RefCounter() :
			refc(0)
		{}

		void incRef()
		{
			refc++;
		}

		int decRef()
		{
			return --refc;
		}
	};

	template< typename T >
	class RefObject : public RefCounter
	{
	public:

		T data;

		RefObject()
		{
			incRef();
		}

		RefObject(T _data)
		{
			data = _data;
			incRef();
		}

		void assign(const T& right)
		{
			incRef();
			data = right;
		}

		operator T () { return data; }
		T& operator -> () { return data; }
	};
}
