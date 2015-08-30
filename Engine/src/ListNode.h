#pragma once
/*
ListNode must be inherited by the base object.

e.g. class GameObject : public ListNode<GameObject>
*/
#include <vector>

#include "Memory.h"
#include "Types.h"

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
			mParent(NULL)
		{}

		virtual ~ListNode()
		{
			TRAVERSE_VECTOR(x, mChild)
				SAFE_DELETE(mChild[x]);
		}

		virtual T* AddChild(T* obj)
		{
			// Check if the object is already a child.
			//
			TRAVERSE_VECTOR(x, mChild)
				if (mChild[x] == obj)
					return obj;

			// Remove the object from its parent.
			//
			if (obj->mParent)
				obj->mParent->RemoveChild(obj);

			// Add the object as a child.
			//
			mChild.push_back(obj);
			obj->mParent = (T*)this;

			return obj;
		}

		virtual void RemoveChild(T* obj)
		{
			TRAVERSE_LIST(it, mChild)
			{
				if (*it == obj)
				{
					obj->mParent = NULL;
					mChild.erase(it);
					return;
				}
			}
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
