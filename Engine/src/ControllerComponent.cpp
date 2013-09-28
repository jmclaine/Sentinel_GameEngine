#include "GameObject.h"
#include "ControllerComponent.h"

namespace Sentinel
{
	ControllerComponent::ControllerComponent()
	{
		mType = GameComponent::CONTROLLER;
	}

	void ControllerComponent::Startup()
	{
		mPhysics = (PhysicsComponent*)mOwner->FindComponent( GameComponent::PHYSICS );

		if( mPhysics == NULL )
			throw AppException( "ControllerComponent::Startup()\n" + std::string( mOwner->mName ) + " does not contain PhysicsComponent" );


		mTransform = (TransformComponent*)mOwner->FindComponent( GameComponent::TRANSFORM );

		if( mTransform == NULL )
			throw AppException( "ControllerComponent::Startup()\n" + std::string( mOwner->mName ) + " does not contain TransformComponent" );
	}

	void ControllerComponent::Update()
	{}

	void ControllerComponent::Shutdown()
	{}
}
