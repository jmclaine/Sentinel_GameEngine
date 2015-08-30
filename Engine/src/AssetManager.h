#pragma once
/*
Each managed type saves and loads the string
designation.  It should use an int to save
memory and not require the unordered_map.
A simple function to convert the string
to a value exists within Util.h called
StringToID; limited to 16 characters.
See function for more details.
*/
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

#include "Util.h"

namespace Sentinel
{
	template<class Type>
	class AssetManager
	{
	protected:

		std::unordered_map<std::string, std::shared_ptr<Type>> mData;

		AssetManager() {}

		virtual ~AssetManager()
		{
			RemoveAll();
		}

	public:

		std::shared_ptr<Type> Add(const std::string& name, std::shared_ptr<Type> data)
		{
			if (!data)
				return data;

			if (mData.find(name) != mData.end())
				return data;

			mData.insert(std::pair<std::string, std::shared_ptr<Type>>(name, data));

			return data;
		}

		void Remove(const std::string& name)
		{
			auto it = mData.find(name);

			if (it != mData.end())
				mData.erase(it);
		}

		void RemoveAll()
		{
			mData.clear();
		}

		std::shared_ptr<Type> Get(const std::string& name)
		{
			auto it = mData.find(name);

			if (it != mData.end())
				return it->second;
			else
				return NULL;
		}

		std::string Get(std::shared_ptr<Type> data)
		{
			TRAVERSE_LIST(it, mData)
			{
				if (it->second.get() == data.get())
					return it->first;
			}

			return "";
		}

		void GetAll(std::vector<std::string>& names, std::vector<std::shared_ptr<Type>>& data)
		{
			data.clear();
			data.reserve(mData.size());

			names.clear();
			names.reserve(mData.size());

			for (auto kv = mData.begin(); kv != mData.end(); ++kv)
			{
				names.push_back(kv->first);
				data.push_back(kv->second);
			}
		}
	};
}
