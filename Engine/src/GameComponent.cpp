#include "GameComponent.h"

namespace Sentinel
{
	GameComponent::GameComponent() :
		mName( NULL )
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

	void GameComponent::Save( FILE* file )
	{
		const char* id = typeid( this ).name();
	}
	
	void GameComponent::Load( FILE* file )
	{}
}
