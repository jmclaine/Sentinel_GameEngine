#pragma once

#include <exception>
#include <typeinfo>

#include "Sentinel.h"
#include "Serializable.h"
#include "Archive.h"

namespace Sentinel
{
	class GameObject;

	class SENTINEL_DLL GameComponent : public Serializable
	{
	protected:

		GameObject* mOwner;

	public:

		bool mEnabled;

	protected:

		GameComponent();

	public:

		virtual ~GameComponent();

		////////////////////////////////

		virtual void Startup();

		virtual void Update();

		virtual void Shutdown();

		////////////////////////////////

		const GameObject* GetOwner();

		virtual void SetOwner(GameObject* owner);

		////////////////////////////////

		virtual void Save(Archive& archive);
		virtual void Load(Archive& archive);

		virtual GameComponent* Copy();

	protected:

		virtual void Copy(GameComponent* component);
	};
}
