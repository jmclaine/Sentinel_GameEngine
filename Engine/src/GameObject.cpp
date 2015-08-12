#include "Util.h"
#include "Timing.h"
#include "GameObject.h"
#include "GameWorld.h"
#include "Archive.h"
#include "Component/Drawable.h"
#include "Component/Camera.h"
#include "Component/Light.h"
#include "Component/Transform.h"
#include "Component/Controller3D.h"
#include "Component/Physics.h"

namespace Sentinel
{
	DEFINE_SERIAL_REGISTER(GameObject);

	GameObject::GameObject(const std::string& name) :
		mTransform(NULL),
		mController(NULL),
		mPhysics(NULL),
		mDrawable(NULL),
		mCamera(NULL),
		mWorld(NULL),
		mName(name),
		mLayer(0x0001),
		mEnabled(true)
	{}

	GameObject::~GameObject()
	{
		TRAVERSE_VECTOR(x, mComponent)
			delete mComponent[x];
	}

	DEFINE_SERIAL_CLONE(GameObject);

	////////////////////////////////////////////////////////////

	GameComponent* GameObject::Attach(GameComponent* component)
	{
		if (!component)
			return NULL;

		component->SetOwner(this);

		mComponent.push_back(component);

		return component;
	}

	GameComponent* GameObject::Detach(GameComponent* component)
	{
		_ASSERT(component);

		TRAVERSE_VECTOR(x, mComponent)
		{
			if (component == mComponent[x])
			{
				mComponent.erase(mComponent.begin() + x);
				break;
			}
		}

		return component;
	}

	////////////////////////////////////////////////////////////

	GameObject* GameObject::AddChild(GameObject* obj)
	{
		if (mWorld)
		{
			mWorld->RemoveGameObject(obj);

			obj->SetWorld(mWorld);
			obj->SetParent(this);
		}

		return ListNode<GameObject>::AddChild(obj);
	}

	////////////////////////////////////////////////////////////

	GameWorld* GameObject::GetWorld()
	{
		return mWorld;
	}

	void GameObject::SetWorld(GameWorld* world)
	{
		mWorld = world;

		TRAVERSE_VECTOR(x, mChild)
		{
			mChild[x]->SetWorld(world);
		}
	}

	void GameObject::SetActive(bool active)
	{
		mEnabled = active;

		if (mDrawable)
			mDrawable->mEnabled = active;

		TRAVERSE_VECTOR(x, mChild)
			mChild[x]->SetActive(active);
	}

	////////////////////////////////////////////////////////////

	void GameObject::Startup()
	{
		TRAVERSE_VECTOR(x, mComponent)
			mComponent[x]->Startup();

		TRAVERSE_VECTOR(x, mChild)
			mChild[x]->Startup();
	}

	void GameObject::UpdateController()
	{
		if (mEnabled)
		{
			if (mController)
				mController->Execute();

			TRAVERSE_VECTOR(x, mChild)
				mChild[x]->UpdateController();
		}
	}

	void GameObject::UpdatePhysics()
	{
		if (mEnabled)
		{
			if (mPhysics)
				mPhysics->Execute();

			TRAVERSE_VECTOR(x, mChild)
				mChild[x]->UpdatePhysics();
		}
	}

	void GameObject::UpdateTransform()
	{
		if (mEnabled)
		{
			if (mTransform)
				mTransform->Execute();

			TRAVERSE_VECTOR(x, mChild)
				mChild[x]->UpdateTransform();
		}
	}

	void GameObject::UpdateComponents()
	{
		if (mEnabled)
		{
			TRAVERSE_VECTOR(x, mComponent)
				mComponent[x]->Update();

			TRAVERSE_VECTOR(x, mChild)
				mChild[x]->UpdateComponents();
		}
	}

	void GameObject::UpdateCamera()
	{
		if (mEnabled)
		{
			if (mCamera)
				mCamera->Execute();

			TRAVERSE_VECTOR(x, mChild)
				mChild[x]->UpdateCamera();
		}
	}

	void GameObject::UpdateDrawable()
	{
		if (mEnabled)
		{
			if (mDrawable)
				mDrawable->Execute();

			TRAVERSE_VECTOR(x, mChild)
				mChild[x]->UpdateDrawable();
		}
	}

	void GameObject::Shutdown()
	{
		TRAVERSE_VECTOR(x, mComponent)
			mComponent[x]->Shutdown();

		TRAVERSE_VECTOR(x, mChild)
			mChild[x]->Shutdown();
	}

	////////////////////////////////////////////////////////////

	void GameObject::Save(Archive& archive)
	{
		SERIAL_REGISTER.Save(archive);

		archive.Write(&mName);

		BYTE size = (BYTE)mComponent.size();
		archive.Write(&size);

		TRAVERSE_VECTOR(x, mComponent)
			mComponent[x]->Save(archive);

		size = (BYTE)mChild.size();
		archive.Write(&size);

		TRAVERSE_VECTOR(x, mChild)
			mChild[x]->Save(archive);
	}

#define LOAD_COMPONENT()\
{\
	component = (GameComponent*)SerialRegister::Load(archive);\
	if (component)\
	{\
		Attach(component);\
		component->Load(archive);\
	}\
}

	void GameObject::Load(Archive& archive)
	{
		archive.Read(&mName);

		int id = 0;
		GameComponent* component;

		BYTE size = 0;
		archive.Read(&size);

		for (BYTE x = 0; x < size; ++x)
			LOAD_COMPONENT();

		archive.Read(&size);

		GameObject* obj;
		for (BYTE x = 0; x < size; ++x)
		{
			obj = (GameObject*)SerialRegister::Load(archive);
			if (obj)
			{
				obj->SetWorld(mWorld);
				obj->Load(archive);
				AddChild(obj);
			}
		}
	}

	GameObject* GameObject::Copy()
	{
		GameObject* obj = new GameObject(mName);

		TRAVERSE_VECTOR(x, mComponent)
			obj->Attach(mComponent[x]->Copy());

		TRAVERSE_VECTOR(x, mChild)
			obj->AddChild(mChild[x]->Copy());

		return obj;
	}
}
