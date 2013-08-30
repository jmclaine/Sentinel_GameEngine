#include "GameObject.h"
#include "TriggerComponent.h"

namespace Sentinel
{
	TriggerComponent::TriggerComponent()
	{
		mPhysics = NULL;
	}

	TriggerComponent::~TriggerComponent()
	{}

	void TriggerComponent::Startup()
	{
		mPhysics = (PhysicsComponent*)mOwner->FindComponent( COMPONENT_TRANSFORM );

		if( mPhysics == NULL )
			throw AppException( "TriggerComponent::Startup()\n" + mOwner->mName + " does not contain PhysicsComponent" );
	}

	void TriggerComponent::Update()
	{}

	void TriggerComponent::Shutdown()
	{
		mPhysics = NULL;
	}
}