#pragma once

#include <exception>
#include <string>
#include <sstream>

namespace Sentinel
{
	class GameObject;

	enum ComponentType
	{
		COMPONENT_GAME,
		COMPONENT_CONTROLLER,
		COMPONENT_COLLISION,
		COMPONENT_PHYSICS,
		COMPONENT_TRANSFORM,
		COMPONENT_CAMERA,
		COMPONENT_LIGHT,
		COMPONENT_DRAWABLE,

		NUM_COMPONENTS
	};

	class GameComponent
	{
	protected:

		GameObject*		mOwner;
		ComponentType	mComponentType;

	public:

		std::string		mName;

		////////////////////////////////

		GameComponent()
		{
			mComponentType = COMPONENT_GAME;
		}

		virtual ~GameComponent() {}

		virtual void Startup() = 0;

		virtual void Update() = 0;

		virtual void Shutdown() = 0;

		void SetOwner( GameObject* owner )
		{
			mOwner = owner;
		}

		ComponentType Type()
		{
			return mComponentType;
		}
	};
}
