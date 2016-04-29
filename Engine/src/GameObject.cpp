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

#include <algorithm>

namespace Sentinel
{
	SerialRegister GameObject::SERIAL_REGISTER("GameObject", GameObject::Clone);
	Serializable* GameObject::Clone() { return new GameObject(); }

	GameObject::GameObject(const std::string& name) :
		mTransform(nullptr),
		mController(nullptr),
		mPhysics(nullptr),
		mDrawable(nullptr),
		mCamera(nullptr),
		mWorld(nullptr),
		mName(name),
		mLayer(0x0001),
		mEnabled(true)
	{ }

	GameObject::~GameObject()
	{ }

	////////////////////////////////////////////////////////////

	void GameObject::AddComponent(GameComponent* component)
	{
		_ASSERT(component);

		component->SetOwner(this);

		mComponents.push_back(std::move(std::unique_ptr<GameComponent>(component)));
	}

	void GameObject::RemoveComponent(GameComponent* component)
	{
		_ASSERT(component);

		for (auto& it = mComponents.begin(); it != mComponents.end(); ++it)
		{
			if ((*it).get() == component)
			{
				mComponents.erase(std::remove(mComponents.begin(), mComponents.end(), *it));
				return;
			}
		}
	}

	////////////////////////////////////////////////////////////

	void GameObject::AddChild(GameObject* obj)
	{
		obj->SetWorld(mWorld);
		obj->SetParent(this);

		ListNode<GameObject>::AddChild(obj);
	}

	////////////////////////////////////////////////////////////

	GameWorld* GameObject::GetWorld()
	{
		return mWorld;
	}

	void GameObject::SetWorld(GameWorld* world)
	{
		mWorld = world;

		for (const auto& child : mChild)
			child->SetWorld(world);
	}

	void GameObject::SetActive(bool active)
	{
		mEnabled = active;

		if (mDrawable)
			mDrawable->mEnabled = active;

		for (const auto& child : mChild)
			child->SetActive(active);
	}

	////////////////////////////////////////////////////////////

	void GameObject::Startup()
	{
		for (const auto& component : mComponents)
			component->Startup();

		for (const auto& child : mChild)
			child->Startup();
	}

	void GameObject::UpdateController()
	{
		if (mEnabled)
		{
			if (mController)
				mController->Execute();

			for (const auto& child : mChild)
				child->UpdateController();
		}
	}

	void GameObject::UpdatePhysics()
	{
		if (mEnabled)
		{
			if (mPhysics)
				mPhysics->Execute();

			for (const auto& child : mChild)
				child->UpdatePhysics();
		}
	}

	void GameObject::UpdateTransform()
	{
		if (mEnabled)
		{
			if (mTransform)
				mTransform->Execute();

			for (const auto& child : mChild)
				child->UpdateTransform();
		}
	}

	void GameObject::UpdateComponents()
	{
		if (mEnabled)
		{
			for (const auto& component : mComponents)
			{
				if (component->mEnabled)
					component->Update();
			}

			for (const auto& child : mChild)
				child->UpdateComponents();
		}
	}

	void GameObject::UpdateCamera()
	{
		if (mEnabled)
		{
			if (mCamera)
				mCamera->Execute();

			for (auto child : mChild)
				child->UpdateCamera();
		}
	}

	void GameObject::Shutdown()
	{
		for (const auto& component : mComponents)
			component->Shutdown();

		for (const auto& child : mChild)
			child->Shutdown();
	}

	////////////////////////////////////////////////////////////

	void GameObject::Save(Archive& archive)
	{
		SERIAL_REGISTER.Save(archive);

		archive.Write(&mName);

		BYTE size = (BYTE)mComponents.size();
		archive.Write(&size);

		for (const auto& component : mComponents)
			component->Save(archive);

		size = (BYTE)mChild.size();
		archive.Write(&size);

		for (const auto& child : mChild)
			child->Save(archive);
	}

	void GameObject::Load(Archive& archive)
	{
		archive.Read(&mName);

		int id = 0;
		GameComponent* component;

		BYTE size = 0;
		archive.Read(&size);

		for (BYTE x = 0; x < size; ++x)
		{
			component = (GameComponent*)SerialRegister::Load(archive);
			if (component)
			{
				AddComponent(component);
				component->Load(archive);
			}
		}

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

		for (const auto& component : mComponents)
			obj->AddComponent(component->Copy());

		for (const auto& child : mChild)
			obj->AddChild(child->Copy());

		return obj;
	}
}
