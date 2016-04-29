#include "GameComponent.h"
#include "GameObject.h"
#include "Archive.h"

namespace Sentinel
{
	GameComponent::GameComponent() :
		mOwner(nullptr),
		mEnabled(true)
	{
		static UINT id = 0;
		mID = id++;
	}

	GameComponent::~GameComponent()
	{ }

	UINT GameComponent::ID()
	{
		return mID;
	}

	const GameObject* GameComponent::GetOwner()
	{
		return mOwner;
	}

	void GameComponent::SetOwner(GameObject* owner)
	{
		mOwner = owner;
	}

	////////////////////////////////

	void GameComponent::Startup()
	{ }

	void GameComponent::Update()
	{ }

	void GameComponent::Shutdown()
	{ }

	////////////////////////////////

	void GameComponent::Save(Archive& archive)
	{ }

	void GameComponent::Load(Archive& archive)
	{ }

	////////////////////////////////

	GameComponent* GameComponent::Copy()
	{
		return nullptr;
	}

	void GameComponent::Copy(GameComponent* component)
	{
		component->mOwner = mOwner;
		component->mEnabled = mEnabled;
	}
}
