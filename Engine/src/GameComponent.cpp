#include "GameComponent.h"
#include "GameObject.h"
#include "Archive.h"

namespace Sentinel
{
	GameComponent::GameComponent() :
		mOwner( NULL ),
		mType( DEFAULT ),
		mEnabled( true )
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

	void GameComponent::SetOwner( GameObject* owner )
	{
		mOwner = owner;
	}

	GameComponent::Type GameComponent::GetType()
	{
		return mType;
	}

	////////////////////////////////

	void GameComponent::Save( Archive& archive )
	{}

	void GameComponent::Load( Archive& archive )
	{}

	GameComponent* GameComponent::Copy()
	{
		return NULL;
	}

	void GameComponent::Copy( GameComponent* component )
	{
		component->mOwner   = mOwner;
		component->mType    = mType;
		component->mEnabled = mEnabled;
	}
}
