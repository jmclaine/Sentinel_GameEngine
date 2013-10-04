#include "GameObject.h"
#include "TriggerComponent.h"

namespace Sentinel
{
	DEFINE_SERIAL_REGISTER( TriggerComponent );
	DEFINE_SERIAL_CLONE( TriggerComponent );

	TriggerComponent::TriggerComponent()
	{
		mType = GameComponent::TRIGGER;

		Shutdown();
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

	/////////////////////////////////

	void TriggerComponent::Save( Archive& archive )
	{
		mSerialRegistry.Save( archive );

		GameComponent::Save( archive );
	}

	void TriggerComponent::Load( Archive& archive )
	{
		GameComponent::Load( archive );
	}
}
