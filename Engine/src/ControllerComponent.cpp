#include "GameObject.h"
#include "ControllerComponent.h"

namespace Sentinel
{
	ControllerComponent::ControllerComponent()
	{
		mComponentType = COMPONENT_CONTROLLER;
	}

	void ControllerComponent::Startup()
	{
		mPhysics = (PhysicsComponent*)mOwner->FindComponent( COMPONENT_PHYSICS );

		if( mPhysics == NULL )
			throw AppException( "ControllerComponent::Startup()\n" + std::string( mOwner->mName ) + " does not contain PhysicsComponent" );


		mTransform = (TransformComponent*)mOwner->FindComponent( COMPONENT_TRANSFORM );

		if( mTransform == NULL )
			throw AppException( "ControllerComponent::Startup()\n" + std::string( mOwner->mName ) + " does not contain TransformComponent" );
	}

	void ControllerComponent::Update()
	{}

	void ControllerComponent::Shutdown()
	{}
}
