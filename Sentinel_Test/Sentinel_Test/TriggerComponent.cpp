#include "GameObject.h"
#include "TriggerComponent.h"

namespace Sentinel
{
	TriggerComponent::TriggerComponent()
	{
		mType = GameComponent::TRIGGER;

		mPhysics = NULL;
	}

	TriggerComponent::~TriggerComponent()
	{}

	void TriggerComponent::Startup()
	{
		mPhysics = (PhysicsComponent*)mOwner->FindComponent( GameComponent::TRANSFORM );

		if( mPhysics == NULL )
			throw AppException( "TriggerComponent::Startup()\n" + std::string( mOwner->mName ) + " does not contain PhysicsComponent" );
	}

	void TriggerComponent::Update()
	{}

	void TriggerComponent::Shutdown()
	{
		mPhysics = NULL;
	}
}
