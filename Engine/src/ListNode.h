#pragma once
/*
ListNode must be inherited by the base object.

e.g. class GameObject : public ListNode<GameObject>
*/
#include <vector>

#include "Memory.h"

namespace Sentinel
{
	template<class T>
	class ListNode
	{
	protected:
		T* mParent;
		std::vector<T*>	mChild;

	public:
		ListNode() :
			mParent(nullptr)
		{ }

		virtual ~ListNode()
		{
			TRAVERSE_VECTOR(x, mChild)
				SAFE_DELETE(mChild[x]);
		}

		virtual void AddChild(T* obj)
		{
			for (auto child : mChild)
				if (child == obj)
					return;

			if (obj->mParent)
				obj->mParent->RemoveChild(obj);

			mChild.push_back(obj);
			obj->mParent = (T*)this;
		}

		virtual void RemoveChild(T* obj)
		{
			mChild.erase(
				std::remove(mChild.begin(), mChild.end(), obj),
				mChild.end());
		}

		T* GetChild(UINT index)
		{
			_ASSERT(index < NumChildren());

			return mChild[index];
		}

		UINT NumChildren()
		{
			return mChild.size();
		}

	protected:
		void SetParent(T* item)
		{
			mParent = item;
		}

	public:
		T* GetParent()
		{
			return mParent;
		}
	};
}
