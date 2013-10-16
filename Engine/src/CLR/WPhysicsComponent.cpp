#include "WPhysicsComponent.h"
#include "WPhysicsSystem.h"

namespace Sentinel { namespace Wrapped
{
	WPhysicsComponent::WPhysicsComponent()
	{
		mRef = new PhysicsComponent();
	}

	WPhysicsComponent::WPhysicsComponent( PhysicsComponent* component )
	{
		mRef = component;
	}

	DEFINE_CAST_COMPONENT( PhysicsComponent );
	
	WRigidBody^ WPhysicsComponent::GetRigidBody()
	{
		return gcnew WRigidBody( static_cast< PhysicsComponent* >(mRef)->GetRigidBody() );
	}

	void WPhysicsComponent::SetRigidBody( WRigidBody^ body )
	{
		static_cast< PhysicsComponent* >(mRef)->SetRigidBody( body->GetRef() );
	}
}}
