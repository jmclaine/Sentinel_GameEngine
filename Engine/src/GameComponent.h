#pragma once

#include <exception>
#include <typeinfo>

#include "Common.h"
#include "Serializable.h"
#include "Archive.h"

namespace Sentinel
{
	class GameObject;

	class SENTINEL_DLL GameComponent : public Serializable
	{
	public:

		enum Type
		{
			DEFAULT,
			CONTROLLER,
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

		bool				mEnabled;

	protected:

		GameComponent();

	public:

		virtual ~GameComponent();

		////////////////////////////////

		virtual void Startup();

		virtual void Update();

		virtual void Shutdown();

		////////////////////////////////

		void SetOwner( GameObject* owner );

		GameComponent::Type GetType();

		////////////////////////////////

		virtual void Save( Archive& archive );
		virtual void Load( Archive& archive );

		virtual GameComponent* Copy();

	protected:

		virtual void Copy( GameComponent* component );
	};
}
