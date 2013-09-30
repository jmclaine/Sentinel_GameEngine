#pragma once

#include <exception>
#include <string>
#include <typeinfo>

#include "Common.h"
#include "Serializeable.h"

namespace Sentinel
{
	class GameObject;

	class SENTINEL_DLL GameComponent : public Serializeable
	{
	public:

		enum Type
		{
			DEFAULT,
			CONTROLLER,
			COLLISION,
			PHYSICS,
			TRANSFORM,
			CAMERA,
			LIGHT,
			DRAWABLE,
			TRIGGER,
		};

	protected:

		GameObject*			mOwner;
		GameComponent::Type	mType;

	public:

		const char*		mName;

		////////////////////////////////

		GameComponent();

		virtual ~GameComponent();

		virtual void Startup() = 0;

		virtual void Update() = 0;

		virtual void Shutdown() = 0;

		////////////////////////////////

		void SetOwner( GameObject* owner );

		GameComponent::Type GetType();
	};
}
