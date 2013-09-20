#include "GameComponent.h"

namespace Sentinel
{
	GameComponent::GameComponent() :
		mName( NULL )
	{
		mComponentType = COMPONENT_GAME;
	}

	GameComponent::~GameComponent()
	{}

	void GameComponent::SetOwner( GameObject* owner )
	{
		mOwner = owner;
	}

	ComponentType GameComponent::Type()
	{
		return mComponentType;
	}
}
