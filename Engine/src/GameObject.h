#pragma once

#include <vector>
#include <typeinfo>
#include <iostream>
#include <algorithm>
#include <memory>
#include <typeindex>

#include "Util.h"
#include "GameComponent.h"
#include "ListNode.h"

namespace Sentinel
{
	namespace Component
	{
		class Camera;
		class Controller;
		class Physics;
		class Transform;
		class Drawable;
	}

	class GameWorld;

	class SENTINEL_DLL GameObject : public Serializable, public ListNode<GameObject>
	{
	private:
		static SerialRegister SERIAL_REGISTER;
		static Serializable* Clone();

		// It is impossible to search for the type of component
		// without iterating over the entire data structure.
		std::vector<std::unique_ptr<GameComponent>> mComponents;

		GameWorld* mWorld;

		bool mEnabled;

	public:
		Component::Controller* mController;
		Component::Physics* mPhysics;
		Component::Transform* mTransform;
		Component::Camera* mCamera;
		Component::Drawable* mDrawable;

		std::string mName;
		WORD mLayer;

		//////////////////////////////

		GameObject(const std::string& name = "GameObject");
		GameObject(const GameObject&) = delete;
		GameObject& operator = (const GameObject&) = delete;
		~GameObject();

		//////////////////////////////

		void AddComponent(GameComponent* component);
		void RemoveComponent(GameComponent* component);

		//////////////////////////////

		void AddChild(GameObject* obj);

		//////////////////////////////

		GameWorld* GetWorld();
		void SetWorld(GameWorld* world);

		void SetActive(bool active);

		//////////////////////////////

		void Startup();

		void UpdateController();
		void UpdatePhysics();
		void UpdateTransform();
		void UpdateComponents();
		void UpdateCamera();

		void Shutdown();

		//////////////////////////////

		template <typename Component = GameComponent>
		Component* GetComponent() const
		{
			for (const auto& item : mComponents)
			{
				auto component = dynamic_cast<Component*>(item.get());
				if (component)
					return component;
			}
			
			return nullptr;
		}

		template <typename Component = GameComponent>
		std::vector<Component*> GetComponents() const
		{
			std::vector<Component*> componentList;

			for (const auto& item : mComponents)
			{
				auto component = dynamic_cast<Component*>(item.get());
				if (component)
					componentList.push_back(component);
			}

			return std::move(componentList);
		}

		//////////////////////////////

		void Save(Archive& archive);
		void Load(Archive& archive);

		GameObject* Copy();
	};
}
