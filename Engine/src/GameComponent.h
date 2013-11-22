#pragma once

#include <exception>
#include <typeinfo>

#include "Common.h"
#include "Serializable.h"

namespace Sentinel
{
	class GameObject;
	class Archive;

	class SENTINEL_DLL GameComponent : public Serializable
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

		std::string			mName;

		////////////////////////////////

		GameComponent();

		virtual ~GameComponent();

		virtual void Startup() = 0;

		virtual void Update() = 0;

		virtual void Shutdown() = 0;

		////////////////////////////////

		void SetOwner( GameObject* owner );

		GameComponent::Type GetType();

		////////////////////////////////

		virtual void Save( Archive& archive );

		virtual void Load( Archive& archive );
	};
}
