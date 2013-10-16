#pragma once

#include "WGameComponent.h"
#include "PhysicsComponent.h"
#include "WPhysicsSystem.h"

namespace Sentinel { namespace Wrapped
{
	public ref class WPhysicsComponent : public WGameComponent
	{
	public:

		WPhysicsComponent();
		WPhysicsComponent( PhysicsComponent* component );

		DECLARE_CAST_COMPONENT( PhysicsComponent );

		WRigidBody^		GetRigidBody();
		void			SetRigidBody( WRigidBody^ body );
	};
}}
