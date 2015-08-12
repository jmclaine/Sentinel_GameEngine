#pragma once

#include <unordered_map>
#include <functional>

#include "Common.h"
#include "Util.h"

#define DELEGATE(type) std::function<type>
#define BIND(func) std::bind(&func, this)

typedef DELEGATE(void()) Action;

namespace Sentinel
{
	class Event
	{
	private:

		std::unordered_map<void(*const*)(), Action> events;

	public:

		Event()
		{}

		void operator += (Action right)
		{
			events[right.target<void(*)()>()] = right;
		}

		void operator -= (Action right)
		{
			events.erase(right.target<void(*)()>());
		}

		void operator -= (void(*const* func)())
		{
			events.erase(func);
		}

		void operator () ()
		{
			if (events.empty())
				return;

			TRAVERSE_LIST(it, events)
				it->second();
		}
	};
}
