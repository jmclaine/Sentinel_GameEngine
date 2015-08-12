#pragma once
/*
All attached GameComponent(s) are freed on Shutdown()
*/
#include <set>
#include <vector>
#include <typeinfo>
#include <iostream>

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

		DECLARE_SERIAL();

		std::vector<GameComponent*> mComponent;

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
		~GameObject();

		//////////////////////////////

		GameComponent* Attach(GameComponent* component);
		GameComponent* Detach(GameComponent* component);

		//////////////////////////////

		GameObject* AddChild(GameObject* obj);	// override

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
		void UpdateDrawable();

		void Shutdown();

		//////////////////////////////

		// Find the first occurrence of a component by type.
		//
		template <typename Component = GameComponent>
		Component* GetComponent()
		{
			_ASSERT((std::is_base_of<GameComponent, Component>::value));

			GameComponent* component;

			TRAVERSE_VECTOR(x, mComponent)
			{
				component = mComponent[x];

				if (dynamic_cast<Component*>(component))
					return (Component*)component;
			}

			return NULL;
		}

		template <typename Component = GameComponent>
		std::vector<Component*> GetComponents()
		{
			_ASSERT((std::is_base_of<GameComponent, Component>::value));

			std::vector<GameComponent*> componentList;
			GameComponent* component;

			TRAVERSE_VECTOR(x, mComponent)
			{
				component = mComponent[x];

				if (dynamic_cast<Component*>(component))
					componentList.push_back((Component*)component);
			}

			return componentList;
		}

		//////////////////////////////

		GameObject* Copy();
	};
}
