#include "Component/Trigger.h"
#include "GameObject.h"
#include "Component/Physics.h"

namespace Sentinel { namespace Component
{
	DEFINE_SERIAL_REGISTER( Trigger );
	DEFINE_SERIAL_CLONE( Trigger );

	Trigger::Trigger() :
		mPhysics( NULL )
	{
		mType = GameComponent::TRIGGER;
	}

	Trigger::~Trigger()
	{}

	void Trigger::Startup()
	{
		mPhysics = (Physics*)mOwner->FindComponent( GameComponent::TRANSFORM );

		if( mPhysics == NULL )
			throw AppException( "Trigger::Startup()\n" + std::string( mOwner->mName ) + " does not contain PhysicsComponent" );
	}

	void Trigger::Update()
	{}

	void Trigger::Shutdown()
	{
		mPhysics = NULL;
	}

	/////////////////////////////////

	void Trigger::Save( Archive& archive )
	{
		SERIAL_REGISTER.Save( archive );

		GameComponent::Save( archive );
	}

	void Trigger::Load( Archive& archive )
	{
		GameComponent::Load( archive );
	}
}}
