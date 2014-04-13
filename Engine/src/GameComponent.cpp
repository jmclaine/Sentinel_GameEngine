#include "GameComponent.h"
#include "GameObject.h"
#include "Archive.h"

namespace Sentinel
{
	GameComponent::GameComponent() :
		mOwner( NULL )
	{
		mType = DEFAULT;
	}

	GameComponent::~GameComponent()
	{}

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
}
