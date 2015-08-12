#include "GameComponent.h"
#include "GameObject.h"
#include "Archive.h"

namespace Sentinel
{
	GameComponent::GameComponent() :
		mOwner(NULL),
		mEnabled(true)
	{}

	GameComponent::~GameComponent()
	{}

	////////////////////////////////

	void GameComponent::Startup()
	{}

	void GameComponent::Update()
	{}

	void GameComponent::Shutdown()
	{}

	////////////////////////////////

	const GameObject* GameComponent::GetOwner()
	{
		return mOwner;
	}

	void GameComponent::SetOwner(GameObject* owner)
	{
		mOwner = owner;
	}

	////////////////////////////////

	void GameComponent::Save(Archive& archive)
	{}

	void GameComponent::Load(Archive& archive)
	{}

	GameComponent* GameComponent::Copy()
	{
		return NULL;
	}

	void GameComponent::Copy(GameComponent* component)
	{
		component->mOwner = mOwner;
		component->mEnabled = mEnabled;
	}
}
