#pragma once

#include <exception>
#include <string>
#include <sstream>

#include "Common.h"

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
		COMPONENT_TRIGGER,

		NUM_COMPONENTS
	};

	class SENTINEL_DLL GameComponent
	{
	protected:

		GameObject*		mOwner;
		ComponentType	mComponentType;

	public:

		std::string		mName;

		////////////////////////////////

		GameComponent();

		virtual ~GameComponent();

		virtual void Startup() = 0;

		virtual void Update() = 0;

		virtual void Shutdown() = 0;

		////////////////////////////////

		void SetOwner( GameObject* owner );

		ComponentType Type();
	};
}
