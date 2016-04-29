#pragma once

#include "Sentinel.h"
#include "Serializable.h"
#include "Archive.h"
#include "RenderLayer.h"

#include <memory>
#include <typeindex>

namespace Sentinel
{
	class GameObject;

	class SENTINEL_DLL GameComponent : public Serializable
	{
	protected:
		UINT mID;
		GameObject* mOwner;

	public:
		bool mEnabled;

	protected:
		explicit GameComponent();
		GameComponent(const GameComponent&) = delete;
		GameComponent& operator = (const GameComponent&) = delete;

	public:
		virtual ~GameComponent();

		////////////////////////////////

		UINT ID();

		const GameObject* GetOwner();
		virtual void SetOwner(GameObject* owner);

	public:
		virtual void Startup();
		virtual void Update();
		virtual void Shutdown();

		////////////////////////////////

		virtual void Save(Archive& archive);
		virtual void Load(Archive& archive);

		virtual GameComponent* Copy();

	protected:
		virtual void Copy(GameComponent* component);
	};
}
