#pragma once
/*
Each managed T saves and loads the string
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
	template<class T>
	class AssetManager
	{
	protected:
		std::unordered_map<std::string, std::shared_ptr<T>> mData;

		AssetManager() { }

		virtual ~AssetManager()
		{
			RemoveAll();
		}

	public:
		void Add(const std::string& name, std::shared_ptr<T>& data)
		{
			auto found = mData.find(name);
			if (found != mData.end())
				throw std::exception("Duplicate name found");

			mData.insert(std::pair<std::string, std::shared_ptr<T>>(name, data));
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

		std::weak_ptr<T> Get(const std::string& name)
		{
			auto found = mData.find(name);

			if (found != mData.end())
				return std::move(std::weak_ptr<T>(found->second));

			throw std::exception("Failed to get asset by name.");
		}

		std::string Get(std::weak_ptr<T>& data)
		{
			auto search = data.lock().get();

			for (const auto& it : mData)
			{
				if (it.second.get() == search)
					return it.first;
			}

			return "";
		}

		void GetAll(std::vector<std::string>& names, std::vector<std::weak_ptr<T>>& data)
		{
			data.clear();
			data.reserve(mData.size());

			names.clear();
			names.reserve(mData.size());

			for (auto asset : mData)
			{
				names.push_back(asset.first);
				data.push_back(asset.second);
			}
		}
	};
}
